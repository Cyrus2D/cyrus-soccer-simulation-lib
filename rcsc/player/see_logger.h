//
// Created by nader on 4/27/24.
//

#ifndef LIBCYRUS_SEE_LOGGER_H
#define LIBCYRUS_SEE_LOGGER_H
#include "player_agent.h"
#include <rcsc/geom/vector_2d.h>
#include <rcsc/geom/angle_deg.h>
#include <rcsc/common/logger.h>

using namespace rcsc;
class SeeLogger {
public:
    void logSee(PlayerAgent & agent_, const VisualSensor & visual_) {
        Vector2D self_pos = agent_.world().self().pos();
        {
            const auto end = visual_.markers().end();
            for (auto it = visual_.markers().begin();
                  it != end;
                  ++it) {
                dlog.addCircle(Logger::SENSOR, self_pos + Vector2D::polar2vector(it->dist_,
                                                                                   it->dir_ +
                                                                                       agent_.world().self().face()),
                                0.5, 0, 0, 0,
                                true);
            }
        }
        {
            const auto end = visual_.behindMarkers().end();
            for (auto it = visual_.behindMarkers().begin();
                  it != end;
                  ++it) {
                dlog.addCircle(Logger::SENSOR, self_pos + Vector2D::polar2vector(it->dist_,
                                                                                   it->dir_ +
                                                                                       agent_.world().self().face()),
                                0.5, 97, 97, 97,
                                true);
            }
        }
        //        has_vel_ = false;
        //        dist_chng_ = 0.0;
        //        dir_chng_ = 0.0;
        //
        //        MovableT::reset();
        //        unum_ = Unum_Unknown;
        //        goalie_ = false;
        //        body_ = VisualSensor::DIR_ERR;
        //        face_ = VisualSensor::DIR_ERR;
        //        arm_ = VisualSensor::DIR_ERR;
        //        kicked_ = false;
        //        tackle_ = false;
        {
            const auto end = visual_.teammates().end();
            for (auto it = visual_.teammates().begin();
                  it != end;
                  ++it) {
                auto pos = self_pos + Vector2D::polar2vector(it->dist_,
                                                              it->dir_ +
                                                                  agent_.world().self().face());
                if (it->body_ != VisualSensor::DIR_ERR) {
                    auto body = it->body_ + agent_.world().self().face();
                    dlog.addSector(Logger::SENSOR, pos, 0, 1.0, body - 90, 180, 60, 67, 247, true);
                }else{
                    dlog.addCircle(Logger::SENSOR, pos, 1.0, 60, 67, 247, true);
                }
                if (it->unum_ != Unum_Unknown) {
                    dlog.addMessage(Logger::SENSOR, pos.x + 0.0, pos.y - 0.8, std::to_string(it->unum_).c_str(), 60, 67,247);
                }
            }
        }
        {
            const auto end = visual_.unknownTeammates().end();
            for (auto it = visual_.unknownTeammates().begin();
                  it != end;
                  ++it) {
                auto pos = self_pos + Vector2D::polar2vector(it->dist_,
                                                              it->dir_ +
                                                                  agent_.world().self().face());
                if (it->body_ != VisualSensor::DIR_ERR) {
                    auto body = it->body_ + agent_.world().self().face();
                    dlog.addSector(Logger::SENSOR, pos, 0, 1.0, body - 90, 180, 81, 209, 255, true);
                }else{
                    dlog.addCircle(Logger::SENSOR, pos, 1.0, 81, 209, 255, true);
                }
                if (it->unum_ != Unum_Unknown) {
                    dlog.addMessage(Logger::SENSOR, pos.x + 0.0, pos.y - 0.8, std::to_string(it->unum_).c_str(), 81, 209, 255);
                }
            }
        }
        {
            const auto end = visual_.opponents().end();
            for (auto it = visual_.opponents().begin();
                  it != end;
                  ++it) {
                auto pos = self_pos + Vector2D::polar2vector(it->dist_,
                                                              it->dir_ +
                                                                  agent_.world().self().face());
                if (it->body_ != VisualSensor::DIR_ERR) {
                    auto body = it->body_ + agent_.world().self().face();
                    dlog.addSector(Logger::SENSOR, pos, 0, 1.0, body - 90, 180, 255, 145, 0, true);
                }else{
                    dlog.addCircle(Logger::SENSOR, pos, 1.0,255, 145, 0, true);
                }
                if (it->unum_ != Unum_Unknown) {
                    dlog.addMessage(Logger::SENSOR, pos.x + 0.0, pos.y - 0.8, std::to_string(it->unum_).c_str(), 255, 145, 0);
                }
            }
        }
        {
            const auto end = visual_.unknownOpponents().end();
            for (auto it = visual_.unknownOpponents().begin();
                  it != end;
                  ++it) {
                auto pos = self_pos + Vector2D::polar2vector(it->dist_,
                                                              it->dir_ +
                                                                  agent_.world().self().face());
                if (it->body_ != VisualSensor::DIR_ERR) {
                    auto body = it->body_ + agent_.world().self().face();
                    dlog.addSector(Logger::SENSOR, pos, 0, 1.0, body - 90, 180, 255, 201, 129, true);
                }else{
                    dlog.addCircle(Logger::SENSOR, pos, 1.0,255, 201, 129, true);
                }
                if (it->unum_ != Unum_Unknown) {
                    dlog.addMessage(Logger::SENSOR, pos.x + 0.0, pos.y - 0.8, std::to_string(it->unum_).c_str(), 255, 201, 129);
                }
            }
        }
        {
            const auto end = visual_.unknownPlayers().end();
            for (auto it = visual_.unknownPlayers().begin();
                  it != end;
                  ++it) {
                auto pos = self_pos + Vector2D::polar2vector(it->dist_,
                                                              it->dir_ +
                                                                  agent_.world().self().face());
                if (it->body_ != VisualSensor::DIR_ERR) {
                    auto body = it->body_ + agent_.world().self().face();
                    dlog.addSector(Logger::SENSOR, pos, 0, 1.0, body - 90, 180, 27, 27, 27, true);
                }else{
                    dlog.addCircle(Logger::SENSOR, pos, 1.0, 27, 27, 27, true);
                }
                if (it->unum_ != Unum_Unknown) {
                    dlog.addMessage(Logger::SENSOR, pos.x + 0.0, pos.y - 0.8, std::to_string(it->unum_).c_str(), 27, 27, 27);
                }
            }
        }
    }
};
#endif //LIBCYRUS_SEE_LOGGER_H
