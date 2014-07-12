/*************************************************************************

  Copyright 2011-2014 Chris Scott (fbscds@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with This program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef __AUDIODECODER_H__
#define __AUDIODECODER_H__

#include <string>
#include <vector>
#include <sndfile.hh>
#include <samplerate.h>
#include "utilities.h"

class AudioDecoder {
    public:
	  bool FileSupported(std::string filename);
      void ReadFile(std::string filename, std::vector<float>& audio);
	protected:
	  void Resample(std::vector<float>& audio, float ratio);
};

#endif //__AUDIODECODER_H__
