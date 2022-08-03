//
// Created by mehkir on 03.08.22.
//

#ifndef VSOMEIP_V3_RECORD_CHECKER_H
#define VSOMEIP_V3_RECORD_CHECKER_H

namespace vsomeip_v3 {
    class record_checker {
      public:
        record_checker();
        // Dummy method always returning true
        bool is_svcb_valid();
        // Dummy method always returning true
        bool is_tlsa_valid();
      protected:
      private:
    };

} // namespace vsomeip_v3

#endif //VSOMEIP_RECORD_CHECKER_H
