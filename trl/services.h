#pragma once

#include "rpc/server.h"
#include "rpc/client.h"

#include <errno.h>


namespace trl {

class TRLService: public rpc::Service {
public:
    enum {
        UPDATE_ACCOUNT = 0x54824fbc,
    };
    int __reg_to__(rpc::Server* svr) {
        int ret = 0;
        if ((ret = svr->reg(UPDATE_ACCOUNT, this, &TRLService::__update_account__wrapper__)) != 0) {
            goto err;
        }
        return 0;
    err:
        svr->unreg(UPDATE_ACCOUNT);
        return ret;
    }
    // these RPC handler functions need to be implemented by user
    // for 'raw' handlers, remember to reply req, delete req, and sconn->release(); use sconn->run_async for heavy job
    virtual void update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount) = 0;
private:
    void __update_account__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i64 in_0;
        req->m >> in_0;
        rpc::i32 in_1;
        req->m >> in_1;
        rpc::i32 in_2;
        req->m >> in_2;
        this->update_account(in_0, in_1, in_2);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
};

class TRLProxy {
protected:
    rpc::Client* __cl__;
public:
    TRLProxy(rpc::Client* cl): __cl__(cl) { }
    rpc::Future* async_update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(TRLService::UPDATE_ACCOUNT, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << txn_id;
            *__cl__ << account_id;
            *__cl__ << amount;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount) {
        rpc::Future* __fu__ = this->async_update_account(txn_id, account_id, amount);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
};

} // namespace trl



namespace trl {

TRLService* create_trl_service();

} // namespace trl

