#ifndef VSOMEIP_V3_SHARED_MEMORY_PARAMETERS
#define VSOMEIP_V3_SHARED_MEMORY_PARAMETERS

#define PUBLISHER_APP_INITIALIZATION         "PUBLISHER_APP_INITIALIZATION"
#define SUBSCRIBER_APP_INITIALIZATION_START  "SUBSCRIBER_APP_INITIALIZATION_START"
#define SUBSCRIBER_APP_INITIALIZATION_END    "SUBSCRIBER_APP_INITIALIZATION_END"
#define GENERATE_OFFER_NONCE_START           "GENERATE_OFFER_NONCE_START"
#define GENERATE_OFFER_NONCE_END             "GENERATE_OFFER_NONCE_END"
#define FIND_SEND                            "FIND_SEND"
#define FIND_RECEIVE                         "FIND_RECEIVE"
#define OFFER_SEND                           "OFFER_SEND"
#define OFFER_RECEIVE                        "OFFER_RECEIVE"
#define SVCB_SERVICE_REQUEST_SEND            "SVCB_SERVICE_REQUEST_SEND"
#define SVCB_SERVICE_REQUEST_RECEIVE         "SVCB_SERVICE_REQUEST_RECEIVE"
#define SVCB_SERVICE_RESPONSE_SEND           "SVCB_SERVICE_RESPONSE_SEND"
#define SVCB_SERVICE_RESPONSE_RECEIVE        "SVCB_SERVICE_RESPONSE_RECEIVE"
#define VALIDATE_OFFER_START                 "VALIDATE_OFFER_START"
#define VALIDATE_OFFER_END                   "VALIDATE_OFFER_END"
#define CLIENT_SIGN_START                    "CLIENT_SIGN_START"
#define CLIENT_SIGN_END                      "CLIENT_SIGN_END"
#define SUBSCRIBE_SEND                       "SUBSCRIBE_SEND"
#define SUBSCRIBE_RECEIVE                    "SUBSCRIBE_RECEIVE"
#define SVCB_CLIENT_REQUEST_SEND             "SVCB_CLIENT_REQUEST_SEND"
#define SVCB_CLIENT_REQUEST_RECEIVE          "SVCB_CLIENT_REQUEST_RECEIVE"
#define SVCB_CLIENT_RESPONSE_SEND            "SVCB_CLIENT_RESPONSE_SEND"
#define SVCB_CLIENT_RESPONSE_RECEIVE         "SVCB_CLIENT_RESPONSE_RECEIVE"
#define TLSA_CLIENT_REQUEST_SEND             "TLSA_CLIENT_REQUEST_SEND"
#define TLSA_CLIENT_REQUEST_RECEIVE          "TLSA_CLIENT_REQUEST_RECEIVE"
#define TLSA_CLIENT_RESPONSE_SEND            "TLSA_CLIENT_RESPONSE_SEND"
#define TLSA_CLIENT_RESPONSE_RECEIVE         "TLSA_CLIENT_RESPONSE_RECEIVE"
#define VERIFY_CLIENT_SIGNATURE_START        "VERIFY_CLIENT_SIGNATURE_START"
#define VERIFY_CLIENT_SIGNATURE_END          "VERIFY_CLIENT_SIGNATURE_END"
#define SUBSCRIBE_ACK_SEND                   "SUBSCRIBE_ACK_SEND"
#define SUBSCRIBE_ACK_RECEIVE                "SUBSCRIBE_ACK_RECEIVE"
#define TLSA_SERVICE_REQUEST_SEND            "TLSA_SERVICE_REQUEST_SEND"
#define TLSA_SERVICE_REQUEST_RECEIVE         "TLSA_SERVICE_REQUEST_RECEIVE"
#define TLSA_SERVICE_RESPONSE_SEND           "TLSA_SERVICE_RESPONSE_SEND"
#define TLSA_SERVICE_RESPONSE_RECEIVE        "TLSA_SERVICE_RESPONSE_RECEIVE"
#define VERIFY_SERVICE_SIGNATURE_START       "VERIFY_SERVICE_SIGNATURE_START"
#define VERIFY_SERVICE_SIGNATURE_END         "VERIFY_SERVICE_SIGNATURE_END"

#define SEGMENT_NAME                    "statistics_shared_memory"
#define TIME_STATISTICS_MAP_NAME        "time_statistics_shared_map"
#define STATISTICS_MUTEX                "statistics_mutex"
#define STATISTICS_CONDITION            "statistics_condition"
#define SEGMENT_SIZE_BYTES              65536

#include <mutex>
#include <chrono>
#include <unordered_map>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <functional>
#include <utility>

//Typedefs of allocators and containers
typedef boost::interprocess::managed_shared_memory::segment_manager                                               segment_manager_t;
typedef boost::interprocess::allocator<void, segment_manager_t>                                                   void_allocator;
typedef std::uint32_t                                                                                             metric_key_t;
typedef std::uint64_t                                                                                             metric_value_t;
typedef std::pair<const metric_key_t, metric_value_t>                                                             metrics_map_value_t;
typedef boost::interprocess::allocator<metrics_map_value_t, segment_manager_t>                                    metrics_map_allocator;
typedef boost::interprocess::map<metric_key_t, metric_value_t, std::less<metric_key_t>, metrics_map_allocator>    metrics_map;

class metrics_map_data {
   public:
      metrics_map metrics_map_;
      metrics_map_data(const void_allocator& void_allocator_instance)
         : metrics_map_(void_allocator_instance)
      {}
};

//Definition of the <host,metrics> map holding an uint32_t as key and metrics_map_data as mapped type
typedef std::uint32_t                                                                                                   host_key_t;
typedef std::pair<const host_key_t, metrics_map_data>                                                                   shared_statistics_map_value_t;
typedef boost::interprocess::allocator<shared_statistics_map_value_t, segment_manager_t>                                shared_statistics_map_allocator;
typedef boost::interprocess::map<host_key_t, metrics_map_data, std::less<host_key_t>, shared_statistics_map_allocator>  shared_statistics_map;

enum time_metric {
    PUBLISHER_APP_INITIALIZATION_,
    SUBSCRIBER_APP_INITIALIZATION_START_,
    SUBSCRIBER_APP_INITIALIZATION_END_,
    GENERATE_OFFER_NONCE_START_,
    GENERATE_OFFER_NONCE_END_,
    FIND_SEND_,
    FIND_RECEIVE_,
    OFFER_SEND_,
    OFFER_RECEIVE_,
    SVCB_SERVICE_REQUEST_SEND_,
    SVCB_SERVICE_REQUEST_RECEIVE_,
    SVCB_SERVICE_RESPONSE_SEND_,
    SVCB_SERVICE_RESPONSE_RECEIVE_,
    VALIDATE_OFFER_START_,
    VALIDATE_OFFER_END_,
    CLIENT_SIGN_START_,
    CLIENT_SIGN_END_,
    SUBSCRIBE_SEND_,
    SUBSCRIBE_RECEIVE_,
    SVCB_CLIENT_REQUEST_SEND_,
    SVCB_CLIENT_REQUEST_RECEIVE_,
    SVCB_CLIENT_RESPONSE_SEND_,
    SVCB_CLIENT_RESPONSE_RECEIVE_,
    TLSA_CLIENT_REQUEST_SEND_,
    TLSA_CLIENT_REQUEST_RECEIVE_,
    TLSA_CLIENT_RESPONSE_SEND_,
    TLSA_CLIENT_RESPONSE_RECEIVE_,
    VERIFY_CLIENT_SIGNATURE_START_,
    VERIFY_CLIENT_SIGNATURE_END_,
    SUBSCRIBE_ACK_SEND_,
    SUBSCRIBE_ACK_RECEIVE_,
    TLSA_SERVICE_REQUEST_SEND_,
    TLSA_SERVICE_REQUEST_RECEIVE_,
    TLSA_SERVICE_RESPONSE_SEND_,
    TLSA_SERVICE_RESPONSE_RECEIVE_,
    VERIFY_SERVICE_SIGNATURE_START_,
    VERIFY_SERVICE_SIGNATURE_END_,
    TIME_METRIC_COUNT_ = VERIFY_SERVICE_SIGNATURE_END_+1
};

#endif /* VSOMEIP_V3_SHARED_MEMORY_PARAMETERS */