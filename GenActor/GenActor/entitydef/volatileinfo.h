/*
   This source file is part of KBEngine
   For the latest info, see http://www.kbengine.org/

   Copyright (c) 2008-2016 KBEngine.

   KBEngine is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   KBEngine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef KBE_VOLATILEINFO_H
#define KBE_VOLATILEINFO_H

#include "common.h"

namespace KBEngine{

    class VolatileInfo 
    {

        public:
            static const float ALWAYS;
            static const float NEVER;

            virtual ~VolatileInfo(){
            }

            void update(bool pos_isALWAYS, bool yaw_isALWAYS, bool pitch_isALWAYS, bool roll_isALWAYS)
            {
                position_ = pos_isALWAYS ? ALWAYS : NEVER;
                yaw_ = yaw_isALWAYS ? ALWAYS : NEVER;
                roll_ = pitch_isALWAYS ? ALWAYS : NEVER;
                pitch_ = roll_isALWAYS ? ALWAYS : NEVER;
            }

            void update(float pos, float yaw, float pitch, float roll)
            {
                position_ = pos;
                yaw_ = yaw;
                roll_ = pitch;
                pitch_ = roll;
            }

            void updateToNEVER()
            {
                position_ = NEVER;
                yaw_ = NEVER;
                roll_ = NEVER;
                pitch_ = NEVER;
            }

            void updateToALWAYS()
            {
                position_ = ALWAYS;
                yaw_ = ALWAYS;
                roll_ = ALWAYS;
                pitch_ = ALWAYS;
            }

            float position() const{ return position_; };
            float yaw() const{ return yaw_; };
            float roll() const{ return roll_; };
            float pitch() const{ return pitch_; };

            void position(float v){ 
                position_ = v; 
            };

            void yaw(float v){ 
                yaw_ = v;
            };

            void roll(float v){ 
                roll_ = v;
            };

            void pitch(float v){ 
                pitch_ = v;
            };

        protected:	
            float position_;
            float yaw_;
            float roll_;
            float pitch_;
    };

}


#endif // KBE_VOLATILEINFO_H
