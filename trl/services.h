#pragma once

#include "rpc/server.h"
#include "rpc/client.h"

#include <errno.h>


namespace trl {

class TRLService: public rpc::Service {
public:
    enum {
        BEGIN_TRANSACTION = 0x213edd31,
        UPDATE_ACCOUNT = 0x6a68af66,
        COMMIT_PREPARE = 0x1420758a,
        COMMIT_CONFIRM = 0x64c14edb,
        ABORT_TRANSACTION = 0x63f4c2eb,
    };
    int __reg_to__(rpc::Server* svr) {
        int ret = 0;
        if ((ret = svr->reg(BEGIN_TRANSACTION, this, &TRLService::__begin_transaction__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(UPDATE_ACCOUNT, this, &TRLService::__update_account__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(COMMIT_PREPARE, this, &TRLService::__commit_prepare__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(COMMIT_CONFIRM, this, &TRLService::__commit_confirm__wrapper__)) != 0) {
            goto err;
        }
        if ((ret = svr->reg(ABORT_TRANSACTION, this, &TRLService::__abort_transaction__wrapper__)) != 0) {
            goto err;
        }
        return 0;
    err:
        svr->unreg(BEGIN_TRANSACTION);
        svr->unreg(UPDATE_ACCOUNT);
        svr->unreg(COMMIT_PREPARE);
        svr->unreg(COMMIT_CONFIRM);
        svr->unreg(ABORT_TRANSACTION);
        return ret;
    }
    // these RPC handler functions need to be implemented by user
    // for 'raw' handlers, remember to reply req, delete req, and sconn->release(); use sconn->run_async for heavy job
    virtual void begin_transaction(const rpc::i64& txn_id) = 0;
    virtual void update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount) = 0;
    virtual void commit_prepare(const rpc::i64& txn_id, rpc::i8* ok) = 0;
    virtual void commit_confirm(const rpc::i64& txn_id) = 0;
    virtual void abort_transaction(const rpc::i64& txn_id) = 0;
private:
    void __begin_transaction__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i64 in_0;
        req->m >> in_0;
        this->begin_transaction(in_0);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
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
    void __commit_prepare__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i64 in_0;
        req->m >> in_0;
        rpc::i8 out_0;
        this->commit_prepare(in_0, &out_0);
        sconn->begin_reply(req);
        *sconn << out_0;
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __commit_confirm__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i64 in_0;
        req->m >> in_0;
        this->commit_confirm(in_0);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __abort_transaction__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i64 in_0;
        req->m >> in_0;
        this->abort_transaction(in_0);
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
    rpc::Future* async_begin_transaction(const rpc::i64& txn_id, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(TRLService::BEGIN_TRANSACTION, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << txn_id;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 begin_transaction(const rpc::i64& txn_id) {
        rpc::Future* __fu__ = this->async_begin_transaction(txn_id);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
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
    rpc::Future* async_commit_prepare(const rpc::i64& txn_id, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(TRLService::COMMIT_PREPARE, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << txn_id;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 commit_prepare(const rpc::i64& txn_id, rpc::i8* ok) {
        rpc::Future* __fu__ = this->async_commit_prepare(txn_id);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *ok;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_commit_confirm(const rpc::i64& txn_id, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(TRLService::COMMIT_CONFIRM, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << txn_id;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 commit_confirm(const rpc::i64& txn_id) {
        rpc::Future* __fu__ = this->async_commit_confirm(txn_id);
        if (__fu__ == nullptr) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_abort_transaction(const rpc::i64& txn_id, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(TRLService::ABORT_TRANSACTION, __fu_attr__);
        if (__fu__ != nullptr) {
            *__cl__ << txn_id;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 abort_transaction(const rpc::i64& txn_id) {
        rpc::Future* __fu__ = this->async_abort_transaction(txn_id);
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

