/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "osal.h"
#include "hal.h"

#include "EOtheErrorManager.h"
#include "EOVtheSystem_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheSystem.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheSystem_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOmempool_alloc_config_t eom_thesystem_mempool_alloc_config_heaposal =
{
    .heap =
    {
        .allocate       = osal_base_memory_new,
        .reallocate     = osal_base_memory_realloc,
        .release        = osal_base_memory_del
    }
};

const eOmempool_cfg_t eom_thesystem_mempool_cfg_osal = 
{
    .mode               = eo_mempool_alloc_dynamic, // we use static allcoation
    .conf               = &eom_thesystem_mempool_alloc_config_heaposal    
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_sys_start(void (*init_fn)(void));

static void s_eom_thecreation(void);

static EOVtaskDerived* s_eom_gettask(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOMtheSystem";

static EOMtheSystem s_eom_system = 
{
    .thevsys        = NULL,               // thevsys
    .osalcfg        = NULL,               // osalcfg
    .tmrmancfg      = NULL,               // tmrmancfg
    .cbkmancfg      = NULL,               // cbkmancfg
    .user_init_fn   = NULL                // user_init_fn
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOMtheSystem * eom_sys_Initialise(const eOmsystem_cfg_t *syscfg, 
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg,
                                         const eOmtimerman_cfg_t *tmrmancfg,
                                         const eOmcallbackman_cfg_t *cbkmancfg)
{
    uint32_t ram08size = 0;
    uint64_t *ram08data = NULL;

    if(NULL != s_eom_system.thevsys) 
    {
        // already initialised
        return(&s_eom_system);
    }


    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg), "eom_sys_Initialise(): NULL syscfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    // verify that we have a valid osalcfg and halcfg. fsalcfg can be NULL
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg->halcfg), "eom_sys_Initialise(): NULL halcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg->osalcfg), "eom_sys_Initialise(): NULL osalcfg", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);

    // mpoolcfg can be NULL: in such a case we use eo_mempool_alloc_dynamic mode
    // errmancfg can be NULL
    // tmrmancfg can be NULL: in such a case we use default values
    // cbkmancfg can be NULL: in such a case we use default values

    
    // mempool and error manager initialised inside here.
    s_eom_system.thevsys = eov_sys_hid_Initialise(mpoolcfg,
                                                  errmancfg,        // error man 
                                                  (eOres_fp_voidfpvoid_t)s_eom_sys_start, s_eom_gettask, 
                                                  osal_system_abstime_get, osal_system_ticks_abstime_set, 
                                                  (eOuint64_fp_void_t)osal_system_nanotime_get,
                                                  hal_sys_irq_disable);

    s_eom_system.halcfg     = syscfg->halcfg;
    s_eom_system.osalcfg    = syscfg->osalcfg;
    s_eom_system.tmrmancfg  = tmrmancfg;
    s_eom_system.cbkmancfg  = cbkmancfg;


#if     defined(HAL_IS_VERSION_2) || defined(HAL_USE_VERSION_2)
    hal_core_init(syscfg->halcfg);
    hal_core_start();

    if(0 != syscfg->codespaceoffset)
    {
        hal_sys_vectortable_relocate(syscfg->codespaceoffset);
    }    
#else   // must be HAL-1     
    uint32_t ram04size = 0;
    uint32_t *ram04data = NULL;
    // initialise hal
    hal_base_memory_getsize(syscfg->halcfg, &ram04size);
    
    if(0 != ram04size)
    {
        ram04data = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, ram04size, 1);
    }

    hal_base_initialise(syscfg->halcfg, ram04data);
    hal_sys_systeminit();

    if(0 != syscfg->codespaceoffset)
    {
        hal_sys_vectortable_relocate(syscfg->codespaceoffset);
    }   
#endif  //HAL_IS_VERSION_2

//    // initialise fsal
//    if(NULL != syscfg->fsalcfg)
//    {
//        fsal_memory_getsize(syscfg->fsalcfg, &ram04size);
//        
//        if(0 != ram04size)
//        {
//            ram04data = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, ram04size, 1);
//        }
//    
//        fsal_initialise(syscfg->fsalcfg, ram04data);
//    }

 
    // initialise osal
    osal_base_memory_getsize(s_eom_system.osalcfg, &ram08size);
    if(0 != ram08size)
    {
        ram08data = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_64bit, ram08size, 1);
    }

    osal_base_initialise(s_eom_system.osalcfg, ram08data);


    return(&s_eom_system);
   
}


extern EOMtheSystem* eom_sys_GetHandle(void)
{
    if(NULL == s_eom_system.thevsys)
    {
        return(NULL);
    }

    return(&s_eom_system);
}    


extern void eom_sys_Start(EOMtheSystem *p, eOvoid_fp_void_t userinit_fn)
{

    eo_errman_Assert(eo_errman_GetHandle(), (NULL != p), "eom_sys_Start(): NULL handle", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);


    s_eom_sys_start(userinit_fn);
}

extern uint32_t eom_sys_GetHeapSize(EOMtheSystem *p)
{
    if(NULL == s_eom_system.thevsys)
    {
        return(0);
    }
#if     defined(HAL_IS_VERSION_2) || defined(HAL_USE_VERSION_2)
	return(s_eom_system.halcfg->syscfg.heapsize);
#else    
    return(s_eom_system.halcfg->sys_heapsize);
#endif
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eom_sys_start(eOvoid_fp_void_t userinit_fn)
{
    // start osal
    s_eom_system.user_init_fn = userinit_fn;
    osal_system_start(s_eom_thecreation);
 
    return(eores_OK);
}


static void s_eom_thecreation(void)
{
    const uint16_t *myused = NULL;
    const uint16_t *myfree = NULL;
    char str[64];
    // we are in osal, now


    // start the services offered by embobj: timer manager and callback manager

    eom_timerman_Initialise(s_eom_system.tmrmancfg);

    eom_callbackman_Initialise(s_eom_system.cbkmancfg);

    osal_info_entities_get_stats(&myused, &myfree);

#if 0    
    snprintf(str, sizeof(str), "uses %d task, %d stack, %d timers, %d mutexes, %d semaphores, %d messageboxes, %d messages", 
                                    myused[osal_info_entity_task], 
                                    myused[osal_info_entity_globalstack], 
                                    myused[osal_info_entity_timer], 
                                    myused[osal_info_entity_mutex], 
                                    myused[osal_info_entity_semaphore], 
                                    myused[osal_info_entity_messagequeue],
                                    myused[osal_info_entity_message]);

    eo_errman_Info(eo_errman_GetHandle(), str, s_eobj_ownname, NULL);
#endif
    
    // run user defined initialisation ...
    if(NULL != s_eom_system.user_init_fn)
    {
        s_eom_system.user_init_fn();
    }
    
    snprintf(str, sizeof(str), "EOMtheSystem: init task completed, system running");
    eo_errman_Info(eo_errman_GetHandle(), str, s_eobj_ownname, &eo_errman_DescrRunningHappily);
    //eOerrmanDescriptor_t desc = {0};
    //memcpy(&desc, &eo_errman_DescrRunningHappily, sizeof(desc));
    //desc.param = 0;
    //eo_errman_Info(eo_errman_GetHandle(), NULL, s_eobj_ownname, &desc);
}


static EOVtaskDerived* s_eom_gettask(void)
{
    osal_task_t *p;
    
    p = osal_task_get(osal_callerTSK);

    if(NULL == p)
    {
        return(NULL);
    }

    return(osal_task_extdata_get(p));
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




