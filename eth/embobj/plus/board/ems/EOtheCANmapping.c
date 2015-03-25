/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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


#include "stdlib.h"
#include "EoCommon.h"

#include "EoCANmapBoards.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANmapping_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


const eOcanmap_cfg_t eo_canmap_DefaultCfg = 
{
    0
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


////static const char s_eobj_ownname[] = "EOtheCANmapping";


 
static EOtheCANmapping s_eo_canmap_singleton = 
{
	.config             =
    {   
        0
    },
    .tobedefined        = 0
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg)
{
    if(NULL == canmapcfg)
    {
        canmapcfg = &eo_canmap_DefaultCfg;    
    }

    memcpy(&s_eo_canmap_singleton.config, canmapcfg, sizeof(eOcanmap_cfg_t));

    return(&s_eo_canmap_singleton);
}



extern EOtheCANmapping* eo_canmap_GetHandle(void) 
{
    return(&s_eo_canmap_singleton);    
}



extern const eOcanmap_canboard_t * eo_canmap_GetBoard(EOtheCANmapping *p, eOcanmap_entitylocation_t loc)
{
    eOcanmap_canboard_t *board = NULL;
    
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(NULL);
//    } 
    
    if(loc.addr > 15)
    {
        return(NULL);
    }
    
    return((const eOcanmap_canboard_t*)eo_canmapping_boards[loc.port][loc.addr]);
}


extern eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eOcanmap_entitylocation_t loc, eObrd_typeandversions_t *detected)
{
    
    eOresult_t res = eores_NOK_generic;
    
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(eores_NOK_generic);
//    } 
    
    if(loc.addr > 15)
    {
        return(eores_NOK_generic);
    }
    
    if(NULL == detected)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOcanmap_canboard_t * board = eo_canmapping_boards[loc.port][loc.addr];  
    
    if(NULL == board)
    {
        return(eores_NOK_generic);
    }
    
    memcpy(&board->detected, detected, sizeof(eObrd_typeandversions_t));
    
    return(eores_OK);
}

// we dont need the ep and entity to retrieve the index. 
extern eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eOcanmap_entitylocation_t loc)
{
    eOprotIndex_t index = EOK_uint08dummy; // init with this value. it changed only if we find a valid index
    
 
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(EOK_uint08dummy);
//    }
    
    if(loc.addr > 15)
    {
        return(EOK_uint08dummy);
    }
    
    const eOcanmap_canboard_t * theboard = eo_canmapping_boards[loc.port][loc.addr];
    
    if(NULL == theboard)
    {   // it means that we dont have such a board at the specified location
        return(EOK_uint08dummy);
    }
    
    
    // ok, we have a board at that location. we just get the index given by loc.insideindex
    // we dont verify vs type of board and type of requested entity
    
    if(eocanmap_insideindex_first == loc.insideindex)
    {
        index = theboard->board.indexofentity[0];
    }
    else if(eocanmap_insideindex_second == loc.insideindex)
    {
        index = theboard->board.indexofentity[1];
    }
    else if(eocanmap_insideindex_none == loc.insideindex)
    {
        index = theboard->board.indexofentity[0];
    }
        
    if(entindexNONE == index)
    {   // in case some board.indexofentity[0] or board.indexofentity[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    
    return(index);            
}

// but if we use ep and entity we surely have cross-check  
extern eOprotIndex_t eo_canmap_GetEntityIndexExtraCheck(EOtheCANmapping *p, eOcanmap_entitylocation_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    eOprotIndex_t index = EOK_uint08dummy; // init with this value. it changed only if we find a valid index
    
 
// useless if port is only one bit    
//    if(loc.port > 1)
//    {
//        return(EOK_uint08dummy);
//    }
    
    if(loc.addr > 15)
    {
        return(EOK_uint08dummy);
    }
    
    const eOcanmap_canboard_t * theboard = eo_canmapping_boards[loc.port][loc.addr];
    
    if(NULL == theboard)
    {   // it means that we dont have such a board at the specified location
        return(EOK_uint08dummy);
    }
    
    // ok, we have a board at that location. is it compatible with endpoint and entity?

    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {   // on can boards there are either joints or motors
            if((eoprot_entity_mc_joint == entity) || (eoprot_entity_mc_motor == entity))
            {   // get the jomoindex. it depends upon the board being mc4 or 1foc
                if(theboard->board.type == eobrd_cantype_1foc) 
                {   // if 1foc it is always on board.indexofentity[0], irrespectively of the value of loc.insideindex 
                    index = theboard->board.indexofentity[0];
                }
                else if(theboard->board.type == eobrd_cantype_mc4)
                {   // if mc4, index depends on value of loc.insideindex. it cn be only first or second. if loc.insideindex is none or even else: nothing is done
                    if(eocanmap_insideindex_first == loc.insideindex)
                    {
                        index = theboard->board.indexofentity[0];
                    }
                    else if(eocanmap_insideindex_second == loc.insideindex)
                    {
                        index = theboard->board.indexofentity[1];
                    }
                }                
            }
            
        } break;
        
        
        case eoprot_endpoint_analogsensors:
        {   // either strain or mais
            if(eoprot_entity_as_strain == entity)
            {   // the board can be only a strain ... the index is always on board.indexofentity[0]
                if(theboard->board.type == eobrd_cantype_strain) 
                {
                    index = theboard->board.indexofentity[0];
                }
            }
            else if(eoprot_entity_as_mais == entity)
            {   // the board can be only a mais ... the index is always on board.indexofentity[0]
                if(theboard->board.type == eobrd_cantype_mais) 
                {
                    index = theboard->board.indexofentity[0];
                }
            }                                
        } break;
 

        case eoprot_endpoint_skin:
        {   // only a skin
            if(eoprot_entity_sk_skin == entity)
            {   // the board can be only a skin board ... the index is on board.indexofentity[0]
                if(theboard->board.type == eobrd_cantype_skin) 
                {
                    index = theboard->board.indexofentity[0];
                }
            }
        } break;        
 
        
        case eoprot_endpoint_management:
        default:
        {   
            index = EOK_uint08dummy;
        } break;
        
    }
    
    if(entindexNONE == index)
    {   // in case some board.indexofentity[0] or board.indexofentity[1] has a entindexNONE value we must be sure to return EOK_uint08dummy
        index = EOK_uint08dummy;
    }
    
    return(index);
}

extern eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eOcanmap_entitylocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype)
{
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id32);
    eOprotEntity_t entity = eoprot_ID2entity(id32);
    eOprotIndex_t index = eoprot_ID2index(id32);
    return(eo_canmap_GetEntityLocation2(p, ep, entity, index, loc, numoflocs, boardtype));
}
    
extern eOresult_t eo_canmap_GetEntityLocation2(EOtheCANmapping *p, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOcanmap_entitylocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype)
{
    eOresult_t res = eores_NOK_generic;
    const eOcanmap_canboard_t *theboard = NULL;
        
    if((NULL == loc) || (NULL == numoflocs))
    {   
        return(res);
    }  
    
        
    // now we retrieve the pointer to board specified inside the tables.
    // in here we can also verify if the board type is coherent with the asked endpoint-entity
    // and we can also retrieve address.
    switch(ep)
    {
        case eoprot_endpoint_motioncontrol:
        {
            if(eoprot_entity_mc_joint == entity)
            {
                if(index < eo_canmapping_joints_numberof)
                {
                    theboard = eo_canmapping_joints[index];
                }                
            }
            else if(eoprot_entity_mc_motor == entity)
            {
                if(index < eo_canmapping_motors_numberof)
                {
                    theboard = eo_canmapping_motors[index];
                }                
            }

            // verification for motion control is equal for joint or motor
            if(NULL != theboard)
            {   // ok, we have a board. now see if it is a 2foc or 4dc
                if((eobrd_cantype_1foc == theboard->board.type) || (eobrd_cantype_mc4 == theboard->board.type))
                {   // ok, correct board type: we retrieve the info
                    *numoflocs = 1;
                    loc->port = theboard->board.location.port;
                    loc->addr = theboard->board.location.addr; 
                    if(index == theboard->board.indexofentity[0])
                    {
                        loc->insideindex = eocanmap_insideindex_first; // for 1foc we could use eocanmap_insideindex_none, but it is ok even in this way
                        res = eores_OK;
                    }
                    else if(index == theboard->board.indexofentity[1])
                    {
                        loc->insideindex = eocanmap_insideindex_second;
                        res = eores_OK;
                    } 
                    //else
                    //{   // the board is correct but none of the inside indices matches the target index. cannot give ok result                        
                    //}
                    if(NULL != boardtype)
                    {
                        *boardtype = (eObrd_cantype_t)theboard->board.type;
                    }
                }               
            }
        } break;
        

        case eoprot_endpoint_analogsensors:
        {
            if(eoprot_entity_as_strain == entity)
            {
                if(index < eo_canmapping_strains_numberof)
                {
                    theboard = eo_canmapping_strains[index];
                    if(NULL != theboard)
                    {
                        if(eobrd_cantype_strain == theboard->board.type)
                        {   // ok, correct board. we retrieve the info
                            *numoflocs = 1;
                            loc->port = theboard->board.location.port;
                            loc->addr = theboard->board.location.addr;             
                            if(index == theboard->board.indexofentity[0])
                            {
                                loc->insideindex = eocanmap_insideindex_none; // if it is a strain we dont care about the insideindex
                                res = eores_OK;
                            }
                            //else
                            //{   // the board is correct but the insideindex0 does not match the target index. cannot give ok result                        
                            //} 
                            if(NULL != boardtype)
                            {
                                *boardtype = (eObrd_cantype_t)theboard->board.type;
                            }
                        }
                    }
                }                
            }
            else if(eoprot_entity_as_mais == entity)
            {
                if(index < eo_canmapping_maises_numberof)
                {
                    theboard = eo_canmapping_maises[index];
                    if(NULL != theboard)
                    {
                        if(eobrd_cantype_mais == theboard->board.type)
                        {   // ok, correct board. we retrieve the info
                            *numoflocs = 1;
                            loc->port = theboard->board.location.port;
                            loc->addr = theboard->board.location.addr;             
                            if(index == theboard->board.indexofentity[0])
                            {
                                loc->insideindex = eocanmap_insideindex_none; // if it is a mais we dont care about the inside index
                                res = eores_OK;
                            }
                            //else
                            //{   // the board is correct but the insideindex0 does not match the target index. cannot give ok result                        
                            //}  
                            if(NULL != boardtype)
                            {
                                *boardtype = (eObrd_cantype_t)theboard->board.type;
                            }                            
                        }
                    }                    
                }                
            }            
        } break;  
        
        case eoprot_endpoint_skin:
        {
            if(eoprot_entity_sk_skin == entity)
            {
                if(index < eo_canmapping_skins_numberof)
                {
                    const eOcanmap_canboard_t * const * theboards = eo_canmapping_skins_boards[index];
                    //theboard = eo_canmapping_skins[index];
                    if(NULL != theboards)
                    {   // ok, we have a valid array of boards. let us see the first one ...
                        theboard = theboards[0];
                        if(NULL != theboard)
                        {   // must verify it is non NULL ... and that it is a skin board
                            if(eobrd_cantype_skin == theboard->board.type)
                            {   // ok, correct board. we retrieve the info
                                *numoflocs = eo_canmapping_skins_boardsinside[index];
                                loc->port = theboard->board.location.port;
                                loc->addr = theboard->board.location.addr; 
                                loc->insideindex = eocanmap_insideindex_none; // if it is a skin we dont care about insideindex
                                res = eores_OK;
                                if(NULL != boardtype)
                                {
                                    *boardtype = (eObrd_cantype_t)theboard->board.type;
                                }
                            }
                        }
                    }
                }                
            }
           
        } break;  
        
        default:
        {
        } break;
        
    }
       
    
    // returns ok or nok depeding on what the function has found
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





