// -*-c++-*-

/*!
  \file player_intercept.h
  \brief intercept predictor for other players Header File
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

#ifndef RCSC_PLAYER_PLAYER_INTERCEPT_H
#define RCSC_PLAYER_PLAYER_INTERCEPT_H

#include <rcsc/geom/vector_2d.h>
#include <vector>
#include "intercept_simulator_player.h"

namespace rcsc {

class PlayerType;
class BallObject;
class PlayerObject;
class WorldModel;

/*!
  \class PlayerIntercept
  \brief intercept predictor for other players
*/
class PlayerIntercept: public InterceptSimulatorPlayer {
private:
    // not used
    PlayerIntercept() = delete;

public:

    /*!
      \brief construct with all variables.
      \param world const reference to the WormdModel instance
      \param ball_pos_cache const reference to the ball position container
    */
    PlayerIntercept( const Vector2D & ball_pos,
                     const Vector2D & ball_vel ) : InterceptSimulatorPlayer( ball_pos, ball_vel ) { }

    /*!
      \brief destructor. nothing to do
    */
    ~PlayerIntercept()
    { }

    //////////////////////////////////////////////////////////
    /*!
      \brief get predicted ball gettable cycle
      \param player const reference to the player object
      \param goalie goalie mode or not
      \param max_cycle max predict cycle. estimation loop is limited to this value.
      \return predicted cycle value
    */
    int simulate( const WorldModel & wm,
                  const PlayerObject & player,
                  const bool goalie ) const;

    bool canReachAfterTurnDashCyrus( const PlayerData & data,
                                     const Vector2D & ball_pos,
                                     const int total_step ) const;
};

}

#endif