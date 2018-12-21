#ifndef ENSURE_SEND_WAS_INVOKED_HPP
#define ENSURE_SEND_WAS_INVOKED_HPP

#include <functional>
#include <utility>

struct SendWasInvoked {};

template <typename Send>
struct EnsureSendWasInvoked
{
	EnsureSendWasInvoked(const Send& send) : send{send}
	{}
	
	template <typename ... Args>
	SendWasInvoked operator()(Args&& ... args) const
	{
		std::invoke(send, std::forward<Args>(args)...);
		return SendWasInvoked{};
	}
	
	const Send& send;
};

#endif
