// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace ImageTest
    {
        class ColorFuncTest : public Test::ITest
        {
        public:
            ColorFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _rgbToHSV(float, float, float);
            void _util();
            void _serialize();
        };
        
    } // namespace ImageTest
} // namespace djv

