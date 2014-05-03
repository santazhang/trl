#include <unordered_map>

#include "services.h"
#include "utils.h"
#include "memdb/txn.h"
#include "memdb/table.h"

using namespace mdb;
using namespace std;

namespace trl {

class TRLServiceImpl: public TRLService {
    Schema* schema_;
    UnsortedTable* tbl_;
    TxnMgrOCC txn_mgr_;
    unordered_map<i64, Txn*> all_txn_;

public:

    TRLServiceImpl() {
        schema_ = new Schema;
        schema_->add_key_column("id", Value::I32);
        schema_->add_column("balance", Value::I32);

        tbl_ = new UnsortedTable(schema_);
        // populate table
        for (int row_id = 0; row_id < max_record_id; row_id++) {
            vector<Value> row_data = {Value((i32) row_id), Value((i32) 0)};
            Row* row = CoarseLockedRow::create(schema_, row_data);
            tbl_->insert(row);
        }
        Log_info("populated %d rows", max_record_id);
    }

    ~TRLServiceImpl() {
        delete tbl_;
        delete schema_;
    }

    virtual void begin_transaction(const rpc::i64& txn_id) {
        verify(all_txn_.find(txn_id) == all_txn_.end());
        Txn* txn = txn_mgr_.start(txn_id);
        insert_into_map(all_txn_, txn_id, txn);
    }

    virtual void update_account(const rpc::i64& txn_id, const rpc::i32& account_id, const rpc::i32& amount) {
        auto it = all_txn_.find(txn_id);
        verify(it != all_txn_.end());
        Txn* txn = it->second;
        ResultSet rs = txn->query(tbl_, Value(account_id));
        verify(rs.has_next());
        Row* row = rs.next();
        Value v;
        txn->read_column(row, 1, &v);
        v = Value((i32) (v.get_i32() + amount));
        txn->write_column(row, 1, v);
    }

    virtual void commit_prepare(const rpc::i64& txn_id, rpc::i8* ok) {
        auto it = all_txn_.find(txn_id);
        verify(it != all_txn_.end());
        TxnOCC* txn = (TxnOCC *) it->second;
        bool outcome = txn->commit_prepare_or_abort();
        if (outcome) {
            *ok = 0;
        } else {
            all_txn_.erase(it);
            delete txn;
            *ok = -1;
        }
    }

    virtual void commit_confirm(const rpc::i64& txn_id) {
        auto it = all_txn_.find(txn_id);
        verify(it != all_txn_.end());
        TxnOCC* txn = (TxnOCC *) it->second;
        txn->commit_confirm();
        all_txn_.erase(it);
        delete txn;
    }

    virtual void abort_transaction(const rpc::i64& txn_id) {
        auto it = all_txn_.find(txn_id);
        verify(it != all_txn_.end());
        Txn* txn = it->second;
        txn->abort();
        all_txn_.erase(it);
        delete txn;
    }
};

TRLService* create_trl_service() {
    return new TRLServiceImpl;
}

} // namespace trl
