/**
 * Group Semantics example C++ implementation.
 * \author Nicholas H.R. Sims
 * \date Apr. 24th, 2024
 * \copyright CC-BY-4.0 (or whatever lets you do whatever you want with this)
 */

#pragma once

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
