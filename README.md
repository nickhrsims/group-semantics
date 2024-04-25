# Group Semantics

## Abstract

Group semantics refers to the relationship between the validity of data inside
a collection and independent of said collection.

To better understand this concept, the following clarifications are made:

- **ADT** is an acronym of Abstract Data Type. Here, an **ADT** refers to a
  **data type** defined in terms of it's valid possible values, the operations
  that can be performed on the data of that type, and the behavior of those
  operations from the point of view of a user.

- A collection-like ADT that supports **group semantics** is called a **group**.

- An element of a **group** is known as a **participant**.

- The relationship between a **participant** and it's associated **group** is
  represented (if by no other means, at minimum) by it's **membership**.

- Inserting a **participant** into a **group** is referred to as **issuing** a
  **membership**.

- Removing a **participant** from a **group** is referred to as **revoking** a
  **membership**.

- A **membership** is itself a distinct **ADT** that supports, at minimum, an
  operation that behaves like a **destructor**.

- A **destuctor** in this context refers to some operation that is
  automatically carried out when an object is intended to be destroyed. It must
  not be required for a user to in any way manually invoke this operation for it
  to be valid, therefore requiring the invocation to be automatic. Not all
  implementation-hosts (programming languages) may support this, and as such
  by definition cannot support **group semantics**.

- The **destructor**-like operation supported by a **membership** must, at
  minimum, **revoke** said **membership** from that **membership's** associated
  **group** (remove it's associated **participant** from it's associated
  **group**).

Under these clarifications, a **group** issues **memberships** whenever a
client attempts to insert data into it. Those data are called
**participants**. Whenever a **membership** is disposed of, or in any way
becomes invalid, the associated **participant** is removed from the **group**
that issued that **membership**.

All of this is a fancy way of saying: A collection that supports **group
semantics** provides some data-lifetime management facilities to it's clients.

## Implementation Example

There are a number of ways to implement a collection with **group semantics**,
but the simplest is to wrap an existing collection-like data type with
operations that meet the necessary requirements. Note that wrapping collections
in this way means that insertion and removal time-and-space complexities of the
underlying data type become the time-and-space complexities of the **group**
itself.

```c++
#include <list>

/**
 * Group Participation Authority.
 *
 * Issues and revokes memberships, and coordinates between member participants.
 *
 * (Just a list wrapper with element lifecycle awareness).
 */
template <typename T> class group {
public:
  /**
   * A handle representing member participation lifecycle.
   *
   * Moving out of scope will forfeit membership and execution participation.
   *
   * Does not support copy or move by design. Lifetime of membership must
   * be shorter than the lifetime of the registered participant.
   */
  struct membership {
    friend group<T>;
    membership(group<T> &p_group_obj, std::list<T>::iterator p_iterator)
        : group_{p_group_obj}, iterator_{p_iterator} {}
    ~membership() { group_.revoke(*this); };

    // --- Cannot copy or move
    membership(const membership &) = delete;
    membership(membership &&) = delete;
    membership &operator=(const membership &) = delete;
    membership &&operator=(membership &&) = delete;

  private:
    group<T> &group_;
    std::list<T>::iterator iterator_;
  };

  /**
   * Issue a membership token to an execution participant.
   */
  membership issue(T t) {
    return membership{*this, participants_.insert(participants_.end(), t)};
  }

  /**
   * Revoke an execution participation token.
   */
  void revoke(membership &m) { participants_.erase(m.iterator_); }

protected:
  std::list<T> participants_;
};
```

## Comments

There are many other ways to implement this. In reality, much of associated
plumbing could be a `Deleter` associated with a specific `std::unique_ptr`. The
problem with `std::unique_ptr` in this context is that it, like other smart
pointers, are intended to convey **ownership semantics**.

The point of **group semantics** is that they should be capable of being
expressed independent of data ownership. Who has what responsibilities of the
**participant** data is irrelevant to a **group**.

**groups** might well hold the values themselves, or they may hold pointers to
data owned by a client, or by a client-of-the-client. The purpose of a
**group** is simply to assist with collecting **participants** in the same
place for later access in a way that ensures the validity of the data being
iterated upon at all times.

Note also that a **group's** definition does not include _how_ that data is
accessed or used later. These things are not important for a collection to
qualify as having **group semantics**
