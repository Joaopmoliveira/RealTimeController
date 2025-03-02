
#include <cassert>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <memory>

struct gps
{	int counter;
	double latitude;
	double longitude;
	double height;
	double velocity[3];
	double acceleration[3];
	double gforce;
	double orientation[3];
	double angular_velocity[3];
	double standard_deviation[3];
};

struct gps_layout 
{	 const size_t counter_address = 0;
	 const size_t counter_size = 4;

	 const size_t latitude_address = 4;
	 const size_t latitude_size = 8;

	 const size_t longitude_address = 12;
	 const size_t longitude_size = 8;

	 const size_t height_address = 20;
	 const size_t height_size = 8;

	 const size_t velocity_address = 28;
	 const size_t velocity_size = 24;

	 const size_t acceleration_address = 52;
	 const size_t acceleration_size = 24;

	 const size_t gforce_address = 76;
	 const size_t gforce_size = 8;

	 const size_t orientation_address = 84;
	 const size_t orientation_size = 24;

	 const size_t angular_velocity_address = 108;
	 const size_t angular_velocity_size = 24;

	 const size_t standard_deviation_address = 132;
	 const size_t standard_deviation_size = 24;

	//message overall size information and multiplicity
	 const size_t multiplicity = 10;
	 const size_t gps_size = 156;
};

void copy_from_gps_to_memory( unsigned char* memory , const gps & tmp)
{ 
	constexpr gps_layout mapping;

	std::memcpy( memory+mapping.counter_address , &tmp.counter , mapping.counter_size );

	std::memcpy( memory+mapping.latitude_address , &tmp.latitude , mapping.latitude_size );

	std::memcpy( memory+mapping.longitude_address , &tmp.longitude , mapping.longitude_size );

	std::memcpy( memory+mapping.height_address , &tmp.height , mapping.height_size );

	std::memcpy( memory+mapping.velocity_address , &tmp.velocity , mapping.velocity_size );

	std::memcpy( memory+mapping.acceleration_address , &tmp.acceleration , mapping.acceleration_size );

	std::memcpy( memory+mapping.gforce_address , &tmp.gforce , mapping.gforce_size );

	std::memcpy( memory+mapping.orientation_address , &tmp.orientation , mapping.orientation_size );

	std::memcpy( memory+mapping.angular_velocity_address , &tmp.angular_velocity , mapping.angular_velocity_size );

	std::memcpy( memory+mapping.standard_deviation_address , &tmp.standard_deviation , mapping.standard_deviation_size );

}


void copy_from_memory_to_gps( const unsigned char*  memory,gps & tmp)
{ 
	constexpr gps_layout mapping;

	std::memcpy( &tmp.counter,memory+mapping.counter_address , mapping.counter_size );

	std::memcpy( &tmp.latitude,memory+mapping.latitude_address , mapping.latitude_size );

	std::memcpy( &tmp.longitude,memory+mapping.longitude_address , mapping.longitude_size );

	std::memcpy( &tmp.height,memory+mapping.height_address , mapping.height_size );

	std::memcpy( &tmp.velocity,memory+mapping.velocity_address , mapping.velocity_size );

	std::memcpy( &tmp.acceleration,memory+mapping.acceleration_address , mapping.acceleration_size );

	std::memcpy( &tmp.gforce,memory+mapping.gforce_address , mapping.gforce_size );

	std::memcpy( &tmp.orientation,memory+mapping.orientation_address , mapping.orientation_size );

	std::memcpy( &tmp.angular_velocity,memory+mapping.angular_velocity_address , mapping.angular_velocity_size );

	std::memcpy( &tmp.standard_deviation,memory+mapping.standard_deviation_address , mapping.standard_deviation_size );

}
struct grayscale_image
{	int counter;
	unsigned char* data = nullptr;
};

struct grayscale_image_layout 
{	 const size_t counter_address = 156;
	 const size_t counter_size = 4;

	 const size_t data_address = 160;
	 const size_t data_size = 5880000;

	//message overall size information and multiplicity
	 const size_t multiplicity = 300;
	 const size_t grayscale_image_size = 5880004;
};

void copy_from_grayscale_image_to_memory( unsigned char* memory , const grayscale_image & tmp)
{ 
	constexpr grayscale_image_layout mapping;

	std::memcpy( memory+mapping.counter_address , &tmp.counter , mapping.counter_size );

	assert( tmp.data!=nullptr);
	std::memcpy( memory+mapping.data_address , tmp.data , mapping.data_size );

}


void copy_from_memory_to_grayscale_image( const unsigned char*  memory,grayscale_image & tmp)
{ 
	constexpr grayscale_image_layout mapping;

	std::memcpy( &tmp.counter,memory+mapping.counter_address , mapping.counter_size );

	assert( tmp.data!=nullptr);
	std::memcpy( tmp.data,memory+mapping.data_address , mapping.data_size );

}
struct watchdog_message
{	size_t counter;
	size_t sensors_receive_timestamp;
	size_t sensors_send_timestamp;
	size_t watchdog_command_timestamp;
	size_t watchdog_sensor_reqst_timestamp;
	size_t watchdog_sensor_receive_timestamp;
	size_t watchdog_client_reqst_timestamp;
	size_t watchdog_client_receive_timestamp;
	size_t client_receive_timestamp;
	size_t gps_counter;
	size_t gps_reading;
	size_t gps_writing;
	size_t grayscale_image_counter;
	size_t grayscale_image_reading;
	size_t grayscale_image_writing;
};

struct watchdog_message_layout 
{	 const size_t counter_address = 5880160;
	 const size_t counter_size = 8;

	 const size_t sensors_receive_timestamp_address = 5880168;
	 const size_t sensors_receive_timestamp_size = 8;

	 const size_t sensors_send_timestamp_address = 5880176;
	 const size_t sensors_send_timestamp_size = 8;

	 const size_t watchdog_command_timestamp_address = 5880184;
	 const size_t watchdog_command_timestamp_size = 8;

	 const size_t watchdog_sensor_reqst_timestamp_address = 5880192;
	 const size_t watchdog_sensor_reqst_timestamp_size = 8;

	 const size_t watchdog_sensor_receive_timestamp_address = 5880200;
	 const size_t watchdog_sensor_receive_timestamp_size = 8;

	 const size_t watchdog_client_reqst_timestamp_address = 5880208;
	 const size_t watchdog_client_reqst_timestamp_size = 8;

	 const size_t watchdog_client_receive_timestamp_address = 5880216;
	 const size_t watchdog_client_receive_timestamp_size = 8;

	 const size_t client_receive_timestamp_address = 5880224;
	 const size_t client_receive_timestamp_size = 8;

	 const size_t gps_counter_address = 5880232;
	 const size_t gps_counter_size = 8;

	 const size_t gps_reading_address = 5880240;
	 const size_t gps_reading_size = 8;

	 const size_t gps_writing_address = 5880248;
	 const size_t gps_writing_size = 8;

	 const size_t grayscale_image_counter_address = 5880256;
	 const size_t grayscale_image_counter_size = 8;

	 const size_t grayscale_image_reading_address = 5880264;
	 const size_t grayscale_image_reading_size = 8;

	 const size_t grayscale_image_writing_address = 5880272;
	 const size_t grayscale_image_writing_size = 8;

	//message overall size information and multiplicity
	 const size_t watchdog_message_size = 120;
};

void copy_from_watchdog_message_to_memory( unsigned char* memory , const watchdog_message & tmp)
{ 
	constexpr watchdog_message_layout mapping;

	std::memcpy( memory+mapping.counter_address , &tmp.counter , mapping.counter_size );

	std::memcpy( memory+mapping.sensors_receive_timestamp_address , &tmp.sensors_receive_timestamp , mapping.sensors_receive_timestamp_size );

	std::memcpy( memory+mapping.sensors_send_timestamp_address , &tmp.sensors_send_timestamp , mapping.sensors_send_timestamp_size );

	std::memcpy( memory+mapping.watchdog_command_timestamp_address , &tmp.watchdog_command_timestamp , mapping.watchdog_command_timestamp_size );

	std::memcpy( memory+mapping.watchdog_sensor_reqst_timestamp_address , &tmp.watchdog_sensor_reqst_timestamp , mapping.watchdog_sensor_reqst_timestamp_size );

	std::memcpy( memory+mapping.watchdog_sensor_receive_timestamp_address , &tmp.watchdog_sensor_receive_timestamp , mapping.watchdog_sensor_receive_timestamp_size );

	std::memcpy( memory+mapping.watchdog_client_reqst_timestamp_address , &tmp.watchdog_client_reqst_timestamp , mapping.watchdog_client_reqst_timestamp_size );

	std::memcpy( memory+mapping.watchdog_client_receive_timestamp_address , &tmp.watchdog_client_receive_timestamp , mapping.watchdog_client_receive_timestamp_size );

	std::memcpy( memory+mapping.client_receive_timestamp_address , &tmp.client_receive_timestamp , mapping.client_receive_timestamp_size );

	std::memcpy( memory+mapping.gps_counter_address , &tmp.gps_counter , mapping.gps_counter_size );

	std::memcpy( memory+mapping.gps_reading_address , &tmp.gps_reading , mapping.gps_reading_size );

	std::memcpy( memory+mapping.gps_writing_address , &tmp.gps_writing , mapping.gps_writing_size );

	std::memcpy( memory+mapping.grayscale_image_counter_address , &tmp.grayscale_image_counter , mapping.grayscale_image_counter_size );

	std::memcpy( memory+mapping.grayscale_image_reading_address , &tmp.grayscale_image_reading , mapping.grayscale_image_reading_size );

	std::memcpy( memory+mapping.grayscale_image_writing_address , &tmp.grayscale_image_writing , mapping.grayscale_image_writing_size );

}


void copy_from_memory_to_watchdog_message( const unsigned char*  memory,watchdog_message & tmp)
{ 
	constexpr watchdog_message_layout mapping;

	std::memcpy( &tmp.counter,memory+mapping.counter_address , mapping.counter_size );

	std::memcpy( &tmp.sensors_receive_timestamp,memory+mapping.sensors_receive_timestamp_address , mapping.sensors_receive_timestamp_size );

	std::memcpy( &tmp.sensors_send_timestamp,memory+mapping.sensors_send_timestamp_address , mapping.sensors_send_timestamp_size );

	std::memcpy( &tmp.watchdog_command_timestamp,memory+mapping.watchdog_command_timestamp_address , mapping.watchdog_command_timestamp_size );

	std::memcpy( &tmp.watchdog_sensor_reqst_timestamp,memory+mapping.watchdog_sensor_reqst_timestamp_address , mapping.watchdog_sensor_reqst_timestamp_size );

	std::memcpy( &tmp.watchdog_sensor_receive_timestamp,memory+mapping.watchdog_sensor_receive_timestamp_address , mapping.watchdog_sensor_receive_timestamp_size );

	std::memcpy( &tmp.watchdog_client_reqst_timestamp,memory+mapping.watchdog_client_reqst_timestamp_address , mapping.watchdog_client_reqst_timestamp_size );

	std::memcpy( &tmp.watchdog_client_receive_timestamp,memory+mapping.watchdog_client_receive_timestamp_address , mapping.watchdog_client_receive_timestamp_size );

	std::memcpy( &tmp.client_receive_timestamp,memory+mapping.client_receive_timestamp_address , mapping.client_receive_timestamp_size );

	std::memcpy( &tmp.gps_counter,memory+mapping.gps_counter_address , mapping.gps_counter_size );

	std::memcpy( &tmp.gps_reading,memory+mapping.gps_reading_address , mapping.gps_reading_size );

	std::memcpy( &tmp.gps_writing,memory+mapping.gps_writing_address , mapping.gps_writing_size );

	std::memcpy( &tmp.grayscale_image_counter,memory+mapping.grayscale_image_counter_address , mapping.grayscale_image_counter_size );

	std::memcpy( &tmp.grayscale_image_reading,memory+mapping.grayscale_image_reading_address , mapping.grayscale_image_reading_size );

	std::memcpy( &tmp.grayscale_image_writing,memory+mapping.grayscale_image_writing_address , mapping.grayscale_image_writing_size );

}

struct Timing{
	const std::chrono::microseconds tick_duration{10000};
	const std::chrono::microseconds sensors_tick_duration{3000};
	const size_t ticks = 10;
};

struct SharedMemoryAccessor{
private:
	boost::interprocess::shared_memory_object shm;
	boost::interprocess::mapped_region region;

	explicit SharedMemoryAccessor() :shm{boost::interprocess::open_only, "MEMORY_REGION", boost::interprocess::read_write}{
		region = boost::interprocess::mapped_region{shm, boost::interprocess::read_write};
	}

public:

	static std::unique_ptr<SharedMemoryAccessor> create(){
		std::unique_ptr<SharedMemoryAccessor> unique = std::unique_ptr<SharedMemoryAccessor>(new SharedMemoryAccessor{});
		return unique;
	}

	~SharedMemoryAccessor(){

	}

	unsigned char* get_shared_memory_address(){
		return static_cast<unsigned char*>(region.get_address());
	}
	inline size_t size(){
		return 5880280;
	}};

