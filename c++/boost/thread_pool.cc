#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/function.hpp>
#include <pthread.h>
#include <vector>
#include <utility>
#include <cstdlib>

#define NB_JOBS 100
#define NB_THREADS 3

///////////////////////////////////////////////////

struct job
{
	const int job_id_;
	
	job(int job_id)
		:
		job_id_(job_id)
	{

	}
	
	void operator()() const
	{
		std::cout << "Working on job " << job_id_ << std::endl;
	}
};

///////////////////////////////////////////////////

struct monitor
{
	std::vector<bool> busy_;
	boost::mutex mutex_;
	boost::condition cond_;
	int nb_actives_;
	
	monitor(int nb_threads)
		:
		busy_(nb_threads,false),
		mutex_(),
		cond_(),
		nb_actives_(0)
	{
		
	}
	
	void notify_inactive(int id) const
	{
		{
			boost::mutex::scoped_lock lock(mutex_);
			busy_[id] = false;
			cond_.notify_all();
			--nb_actives_;
		}
	}
	
	void notify_active(int id) const
	{
		{
			boost::mutex::scoped_lock lock(mutex_);
			busy_[id] = true;
			++nb_actives_;
		}
	}
	
	int wait_for_inactives() const
	{
		assert( nb_actives_ >= 0 );
		boost::mutex::scoped_lock lock(mutex_);
		if( nb_actives_ == NB_THREADS )
		{
			cond_.wait(lock);
		}
		
		std::vector<bool>::iterator it;
		int i = 0;
		for( it = busy_.begin() ; it != busy_.end() ; ++it , ++i )
		{
			if( !*it )
			{
				return i;
			}
		}
		
		throw std::domain_error	("Inactive thread not found.");
		return 0;
	}
};


///////////////////////////////////////////////////

struct thread_controller
{
	const int thread_id_;
	job* job_to_do_;
	bool asked_to_stop_;
	monitor& mon_;
	boost::condition cond_;
	boost::mutex mutex_;
	boost::mutex job_mutex_;

	thread_controller( 	int thread_id,
						const monitor& mon)
		:
		thread_id_(thread_id),
		job_to_do_(NULL),
		asked_to_stop_(false),
		mon_(mon),
		cond_(),
		mutex_()
	{
		
	}
		
	void stop() 
	{
		boost::mutex::scoped_lock lock(mutex_);
		asked_to_stop_ = true;
		cond_.notify_one();
	}
	
	void set_job(job* j)
	{
		std::cout << "Setting a job." << std::endl;
		boost::mutex::scoped_lock lock(mutex_);
		job_to_do_ = j;
		cond_.notify_all();
	}

	void operator()()
	{
		while(true)
		{
			{
				boost::mutex::scoped_lock lock(mutex_);
				if( job_to_do_ == NULL && !asked_to_stop_ )	
				{
					cond_.wait(lock);
					std::cout << "After the condition." << std::endl;
				}
			}

			if( asked_to_stop_ )
			{
				std::cout << "Asked to stop." << std::endl;
				break;
			}

			if( job_to_do_ == NULL)
			{
				continue;
			}
			std::cout << "Launching job." << std::endl;
			(*job_to_do_)();
			std::cout << "Job finished." << std::endl;
			job_to_do_ = NULL;
			mon_.notify_inactive(thread_id_);
		}
	}
};

///////////////////////////////////////////////////

typedef std::vector<job*> 
	job_pool_t;

typedef  std::vector< std::pair	< thread_controller*, boost::thread* > > 
	thread_pool_t;

int main()
{
	std::cout << "========================================" << std::endl;
	
	// list of jobs
	job_pool_t jobs;
	for( int i = 0 ; i < NB_JOBS ; ++i )
	{
		jobs.push_back(new job(i));
	}

	// to monitor threads
	monitor mon(NB_THREADS);

	// pool of threads
	thread_pool_t threads;
	for( int i = 0 ; i < NB_THREADS ; ++i )
	{
		// to pass a reference on our instanciated controller to boost::thread, to avoid a copy
		thread_controller* tc = new thread_controller(i,mon);
		boost::function<void (void)>* f_controller = new boost::function<void (void)>;
		*f_controller = boost::ref(*tc);
		threads.push_back( std::make_pair( 	tc,
											new boost::thread(*f_controller) ));
	}

	std::vector<job*>::iterator jobs_it;
	for( 	jobs_it = jobs.begin();
			jobs_it != jobs.end();
			++jobs_it)
	{
		int next_inactive = mon.wait_for_inactives();
		mon.notify_active(next_inactive);
		(threads[next_inactive].first)->set_job(*jobs_it);
	}

	thread_pool_t::iterator join_it;
	for( join_it = threads.begin() ; join_it != threads.end() ; ++ join_it )
	{
		(*join_it).first->stop();
		(*join_it).second->join();
	}

	// pointers to delete!!!

	std::cout << "========================================" << std::endl;

	return EXIT_SUCCESS;
}