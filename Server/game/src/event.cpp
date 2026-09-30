#include "stdafx.h"
#include "event_queue.h"

extern void ContinueOnFatalError();
extern void ShutdownOnFatalError();

#ifdef M2_USE_POOL
MemoryPool event_info_data::pool_;
static ObjectPool<EVENT> event_pool;
#endif

static CEventQueue cxx_q;

LPEVENT event_create_ex(TEVENTFUNC func, event_info_data* info, long when)
{
	LPEVENT new_event = NULL;
	
	if (when < 1)
		when = 1;

#ifdef M2_USE_POOL
	new_event = event_pool.Construct();
#else
	new_event = M2_NEW event;
#endif

	assert(NULL != new_event);
	
	// Initialize reference count
	new_event->ref_count = 1;
	new_event->func = func;
	new_event->info = info;
	new_event->q_el = cxx_q.Enqueue(new_event, when, thecore_heart->pulse);
	new_event->is_processing = FALSE;
	new_event->is_force_to_end = FALSE;
	
	return new_event;
}

void event_cancel(LPEVENT * ppevent)
{
	LPEVENT event;
	
	if (!ppevent)
	{
		sys_err("event_cancel: null pointer");
		return;
	}
	
	if (!(event = *ppevent))
		return;

	// CRITICAL FIX: Use get() to access raw pointer for intrusive_ptr
	if (event.get())
	{
		intrusive_ptr_add_ref(event.get());
	}
	
	if (event->is_processing)
	{
		event->is_force_to_end = TRUE;
		if (event->q_el)
		{
			event->q_el->bCancel = TRUE;
		}
		*ppevent = NULL;
		
		// Release our temporary reference
		if (event.get())
		{
			intrusive_ptr_release(event.get());
		}
		return;
	}

	if (!event->q_el)
	{
		*ppevent = NULL;
		if (event.get())
		{
			intrusive_ptr_release(event.get());
		}
		return;
	}

	if (event->q_el->bCancel)
	{
		*ppevent = NULL;
		if (event.get())
		{
			intrusive_ptr_release(event.get());
		}
		return;
	}

	// Mark for cancellation
	event->q_el->bCancel = TRUE;
	*ppevent = NULL;
	
	// Release our temporary reference
	if (event.get())
	{
		intrusive_ptr_release(event.get());
	}
}

void event_reset_time(LPEVENT event, long when)
{
	if (!event || !event.get())
	{
		sys_err("event_reset_time: null event");
		return;
	}

	// SAFETY: Add reference during reset
	intrusive_ptr_add_ref(event.get());
	
	if (!event->is_processing)
	{
		if (event->q_el)
			event->q_el->bCancel = TRUE;
			
		event->q_el = cxx_q.Enqueue(event, when, thecore_heart->pulse);
	}
	
	// Release temporary reference
	intrusive_ptr_release(event.get());
}

int event_process(int pulse)
{
	long new_time;
	int num_events = 0;

	while (pulse >= cxx_q.GetTopKey())
	{
		TQueueElement* pElem = cxx_q.Dequeue();
		
		if (!pElem)
			break;

		if (pElem->bCancel)
		{
			// CRITICAL FIX: No need to manually release - intrusive_ptr handles it
			cxx_q.Delete(pElem);
			continue;
		}

		new_time = pElem->iKey;
		LPEVENT the_event = static_cast<LPEVENT>(pElem->pvData);
		
		if (!the_event || !the_event.get())
		{
			sys_err("event_process: null event in queue element");
			cxx_q.Delete(pElem);
			continue;
		}

		// CRITICAL FIX: Store a copy of intrusive_ptr to keep event alive
		LPEVENT event_holder = the_event;
		
		long processing_time = event_processing_time(the_event);
		cxx_q.Delete(pElem);
		
		the_event->is_processing = TRUE;
		the_event->q_el = NULL;

		if (!the_event->info)
		{
			sys_err("event_process: event with null info, continuing");
			the_event->is_processing = FALSE;
			
			ContinueOnFatalError();
		}
		else
		{
			// Call the event function
			new_time = (the_event->func)(get_pointer(the_event), processing_time);
			
			if (new_time > 0 && !the_event->is_force_to_end)
			{
				// Re-enqueue the event
				the_event->q_el = cxx_q.Enqueue(the_event, new_time, pulse);
				the_event->is_processing = FALSE;
			}
			else
			{
				// Event finished
				the_event->is_processing = FALSE;
			}
		}
		// event_holder goes out of scope here, automatically releasing reference

		++num_events;
	}

	return num_events;
}

long event_processing_time(LPEVENT event)
{
	if (!event || !event.get() || !event->q_el)
		return 0;

	long start_time = event->q_el->iStartTime;
	return (thecore_heart->pulse - start_time);
}

long event_time(LPEVENT event)
{
	if (!event || !event.get() || !event->q_el)
		return 0;

	long when = event->q_el->iKey;
	return (when - thecore_heart->pulse);
}

void event_destroy(void)
{
	TQueueElement* pElem;
	
	while ((pElem = cxx_q.Dequeue()))
	{
		// No manual cleanup needed - intrusive_ptr handles reference counting
		cxx_q.Delete(pElem);
	}
}

int event_count()
{
	return cxx_q.Size();
}

void intrusive_ptr_add_ref(EVENT* p) 
{
	if (!p)
	{
		sys_err("intrusive_ptr_add_ref: null pointer");
		return;
	}
	
	++(p->ref_count);
}

void intrusive_ptr_release(EVENT* p) 
{
	if (!p)
	{
		sys_err("intrusive_ptr_release: null pointer");
		return;
	}
	
	if (p->ref_count <= 0)
	{
		sys_err("intrusive_ptr_release: invalid ref_count %d", p->ref_count);
		return;
	}
	
	if (--(p->ref_count) == 0) 
	{
		// CRITICAL FIX: Clear info before destruction to prevent double-delete
		if (p->info)
		{
			p->info = NULL;
		}
		
#ifdef M2_USE_POOL
		event_pool.Destroy(p);
#else
		M2_DELETE(p);
#endif
	}
}

// ADDITIONAL SAFETY FUNCTIONS

bool event_is_valid(LPEVENT event)
{
	if (!event || !event.get())
		return false;
		
	if (event->ref_count <= 0)
		return false;
		
	return true;
}

void event_safe_cancel(LPEVENT* ppevent)
{
	if (!ppevent || !(*ppevent) || !(*ppevent).get())
		return;
		
	event_cancel(ppevent);
}

//martysama0134's 3f2ef26600e1ab05e646bd9d97d6bc1f