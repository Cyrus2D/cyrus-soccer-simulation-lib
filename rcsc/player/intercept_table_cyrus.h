// -*-c++-*-

/*!
  \file intercept_table.h
  \brief interception info holder Header File
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef RCSC_PLAYER_INTERCEPT_TABLE_CYRUS_H
#define RCSC_PLAYER_INTERCEPT_TABLE_CYRUS_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/game_time.h>
#include <rcsc/player/intercept.h>
#include "intercept_table.h"
#include <vector>
#include <map>

namespace rcsc {

class AbstractPlayerObject;
class PlayerObject;
class WorldModel;

/*-------------------------------------------------------------------*/

/*!
  \class Intercept
  \brief interception data
*/

/*-------------------------------------------------------------------*/

/*!
  \class InterceptTableCyrus
  \brief interception info holder for all players
*/
class InterceptTableCyrus: public InterceptTable {
private:

    //! predicted min reach step for self without stamina exhaust
    int M_self_step_tackle;
    int M_self_exhaust_step_tackle;
    std::vector< Intercept > M_self_results_tackle;

    // not used
    InterceptTableCyrus( const InterceptTableCyrus & ) = delete;
    InterceptTableCyrus & operator=( const InterceptTableCyrus & ) = delete;
public:
    /*!
      \brief init member variables, reserve cache vector memory
    */
    InterceptTableCyrus();

    /*!
      \brief destructor. nothing to do
    */
    virtual
        ~InterceptTableCyrus()
    { }

    /*!
      \brief get minimal ball gettable step for self without stamina exhaust
      \return step value to get the ball
    */
    int selfStepTackle() const
    {
        return M_self_step_tackle;
    }
    /*!
      \brief get minimal ball gettable step for self with stamina exhaust
      \return step value to get the ball
    */
    int selfExhaustStepTackle() const
    {
        return M_self_exhaust_step_tackle;
    }


    const std::vector< Intercept > & selfResultsTackle() const
    {
        return M_self_results_tackle;
    }

private:
    /*!
      \brief clear all cached data
    */
    void clear() override;

    /*!
      \brief predict self interception
      \param wm const reference to the world model
    */
    void predictSelf( const WorldModel & wm ) override;

    /*!
      \predict teammate interception
      \param wm const reference to the world model
    */
    void predictTeammate( const WorldModel & wm ) override;

    /*!
      \predict opponent interception
      \param wm const reference to the world model
    */
    void predictOpponent( const WorldModel & wm ) override;
};

}

#endif
