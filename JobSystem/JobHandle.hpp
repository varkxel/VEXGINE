#ifndef WYEVERN_JOBHANDLE_INCLUDED
#define WYEVERN_JOBHANDLE_INCLUDED

#include "Global/Architecture.hpp"

#include "Job.hpp"

#include <atomic>
#include <array>
#include <bit>
#include <memory>
#include <optional>

namespace Wyevern::Jobs
{
	struct JobHandle
	{
		template<typename JobType>
		JobHandle
		(
			const JobType& job,
			std::optional<std::shared_ptr<JobHandle>> parent = std::nullopt
		):
			data(job),
			unfinished(1)
		{
			if(parent.has_value())
			{
				++(parent.value()->unfinished);
			}
			this->parent = parent;
		}

		JobHandle();
		JobHandle(const JobHandle& other);
		JobHandle& operator=(const JobHandle& other);
		virtual ~JobHandle() {}

		/// <summary>
		/// The amount of unfinished children this job has, plus itself.
		/// </summary>
		std::atomic_llong unfinished;

		/// <summary>
		/// The job which is this job's parent.
		/// </summary>
		std::optional<std::shared_ptr<JobHandle>> parent;

		union DataContainer
		{
			friend struct JobHandle;
		private:
			static constexpr auto PaddingSize = Wyevern::Architecture::FalseSharingMitigation
				- sizeof(parent)
				- sizeof(unfinished);

			std::unique_ptr<Job> reference;
			std::array<std::byte, PaddingSize> data;
		public:
			template<typename JobType>
			static constexpr bool IsStoredLocally()
			{
				return sizeof(JobType) <= PaddingSize;
			}

			DataContainer();
			DataContainer(const DataContainer& other);
			DataContainer& operator=(const DataContainer& other);
			~DataContainer() {}

			template<typename JobType>
			explicit DataContainer(const JobType& job)
			{
				if constexpr(IsStoredLocally<JobType>())
				{
					// Can store data in the job struct
					data = std::bit_cast<std::array<std::byte, PaddingSize>>(job);
				}
				else
				{
					// Cannot store the data in the job struct, store a pointer to it instead.
					reference = std::make_unique<JobType>(job);
				}
			}

			template<typename JobType>
			JobType& Get()
			{
				if constexpr(IsStoredLocally<JobType>())
				{
					// Data is in the job struct
					return reinterpret_cast<JobType&>(&data);
				}
				else
				{
					// Data is in the pointer
					return dynamic_cast<JobType&>(reference.get());
				}
			}
		private:
			static void CopyOpImpl(const DataContainer& src, DataContainer& dest);
		};
		DataContainer data;
	};
}

#endif
