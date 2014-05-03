#include "services.h"

namespace trl {

class TRLServiceImpl: public TRLService {
public:
    virtual void update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount) {
        // TODO
    }
};

TRLService* create_trl_service() {
    return new TRLServiceImpl;
}

} // namespace trl
