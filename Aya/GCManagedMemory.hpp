#pragma once

#include <boost/intrusive/list.hpp>

namespace aya {

using ListBaseHook =
    boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;

class GCManagedMemory: public ListBaseHook {
public:
    virtual ~GCManagedMemory() = default;

    void mark() const {
        // TODO change to iterative. Have to protect ourselves from malicious stack overflow
        if (!marked) {
            markImpl();
            marked = true;
        }
    }

    bool sweep() const {
        if (marked) {
            marked = false;
            return false;
        } else {
            sweepImpl();
            return true;
        }
    }

protected:
    GCManagedMemory() = default;

    virtual void markImpl() const {}
    virtual void sweepImpl() const {}

private:
    mutable bool marked = false;
};

using GCManagedMemoryList =
    boost::intrusive::list<GCManagedMemory, boost::intrusive::constant_time_size<false>>;

}

