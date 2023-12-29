// Copyright (C) 2014-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef VSOMEIP_V3_SD_SERVICE_DISCOVERY_HPP_
#define VSOMEIP_V3_SD_SERVICE_DISCOVERY_HPP_

#include <boost/asio/ip/address.hpp>

#include <vsomeip/primitive_types.hpp>
#include <vsomeip/enumeration_types.hpp>
#include <vsomeip/handler.hpp>
#include "../../routing/include/serviceinfo.hpp"
#include "../../endpoints/include/endpoint.hpp"
#include "../include/service_discovery_host.hpp"

//Additional includes for service authentication
#include "../../dnssec/include/dns_resolver.hpp"
#include "../../dnssec/include/svcb_resolver.hpp"
#include "../../dnssec/include/tlsa_resolver.hpp"
#include "../../service_authentication/include/svcb_cache.hpp"
#include "../../service_authentication/include/challenge_nonce_cache.hpp"
#include "../../service_discovery/include/resume_process_offerservice_cache.hpp"

#ifdef WITH_CLIENT_AUTHENTICATION
#include "../../service_authentication/include/eventgroup_subscription_cache.hpp"
#endif

#include "../../service_authentication/include/eventgroup_subscription_ack_cache.hpp"
//Additional include for time measurement
#include "../../timestamps/include/timestamp_collector.hpp"

#if defined(WITH_ENCRYPTION) && defined(WITH_CLIENT_AUTHENTICATION)
//Additional include for payload encryption key agreement
#include "../../dh_ecc/include/dh_ecc.hpp"
#include "../../dh_ecc/include/encrypted_group_secret_result_cache.hpp"
#endif

namespace vsomeip_v3 {

class configuration;
class eventgroupinfo;

namespace sd {

class service_discovery {
public:
    virtual ~service_discovery() {
    }

    virtual boost::asio::io_context &get_io() = 0;

    virtual void init() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void request_service(service_t _service, instance_t _instance,
            major_version_t _major, minor_version_t _minor, ttl_t _ttl) = 0;
    virtual void release_service(service_t _service, instance_t _instance) = 0;

    virtual void subscribe(service_t _service, instance_t _instance,
            eventgroup_t _eventgroup, major_version_t _major,
            ttl_t _ttl, client_t _client,
            const std::shared_ptr<eventgroupinfo>& _info) = 0;
    virtual void unsubscribe(service_t _service, instance_t _instance,
            eventgroup_t _eventgroup, client_t _client) = 0;
    virtual void unsubscribe_all(service_t _service, instance_t _instance) = 0;
    virtual void unsubscribe_all_on_suspend() = 0;

    virtual bool send(bool _is_announcing) = 0;

    virtual void on_message(const byte_t *_data, length_t _length,
            const boost::asio::ip::address &_sender,
            bool _is_multicast) = 0;

    virtual void on_endpoint_connected(
            service_t _service, instance_t _instance,
            const std::shared_ptr<endpoint> &_endpoint) = 0;

    virtual void offer_service(const std::shared_ptr<serviceinfo> &_info) = 0;
    virtual bool stop_offer_service(const std::shared_ptr<serviceinfo> &_info, bool _send) = 0;
    virtual bool send_collected_stop_offers(const std::vector<std::shared_ptr<serviceinfo>> &_infos) = 0;

    virtual void set_diagnosis_mode(const bool _activate) = 0;

    virtual bool get_diagnosis_mode() = 0;

    virtual void update_remote_subscription(
            const std::shared_ptr<remote_subscription> &_subscription) = 0;

    virtual void register_sd_acceptance_handler(
            const sd_acceptance_handler_t &_handler) = 0;
    virtual void register_reboot_notification_handler(
            const reboot_notification_handler_t &_handler) = 0;
    virtual std::recursive_mutex& get_subscribed_mutex() = 0;

    // Addition for Service Authentication Start #########################################################################
    virtual void set_dns_resolver(dns_resolver* _dns_resolver) = 0;
    virtual void set_svcb_resolver(std::shared_ptr<svcb_resolver> _svcb_resolver) = 0;
    virtual void set_tlsa_resolver(std::shared_ptr<tlsa_resolver> _tlsa_resolver) = 0;
    virtual void set_challenge_nonce_cache(std::shared_ptr<challenge_nonce_cache> _challenge_nonce_cache) = 0;
    virtual void set_svcb_cache(svcb_cache* _svcb_cache) = 0;
    virtual void set_resume_process_offerservice_cache(resume_process_offerservice_cache* _resume_process_offerservice_cache) = 0;

#ifdef WITH_CLIENT_AUTHENTICATION
    virtual void set_eventgroup_subscription_cache(eventgroup_subscription_cache* _eventgroup_subscription_cache) = 0;
#endif

    virtual void set_eventgroup_subscription_ack_cache(eventgroup_subscription_ack_cache* _eventgroup_subscription_ack_cache) = 0;
    virtual void validate_offer(service_t _service, instance_t _instance, major_version_t _major, minor_version_t _minor) = 0;
    virtual void validate_subscribe_ack_and_verify_signature(boost::asio::ip::address_v4 _sender_ip_address, service_t _service, instance_t _instance, major_version_t _major) = 0;
    // Addition for Service Authentication End ###########################################################################
    // Addition for Time Measurement Start ###############################################################################
    virtual void set_timestamp_collector(timestamp_collector* _timestamp_collector) = 0;
    // Addition for Time Measurement End #################################################################################
    
#if defined(WITH_ENCRYPTION) && defined(WITH_CLIENT_AUTHENTICATION)
    // Aditional methods for payload encryption Start ####################################################################
    virtual void set_dh_ecc(std::shared_ptr<dh_ecc> _dh_ecc) = 0;
    virtual void set_group_secret_map(std::shared_ptr<std::map<std::tuple<service_t, instance_t>, CryptoPP::SecByteBlock>> _group_secrets) = 0;
    virtual void set_encrypted_group_secret_result_cache(std::shared_ptr<encrypted_group_secret_result_cache> _encrypted_group_secret_result_cache) = 0;
    // Aditional methods for payload encryption End ######################################################################
#endif

#ifndef WITH_SOMEIP_SD
// Addition for w/o SOME/IP SD Start #######################################################
    virtual void mimic_offerservice_serviceentry(
            service_t _service, instance_t _instance, major_version_t _major,
            minor_version_t _minor, const boost::asio::ip::address &_address,
            uint16_t _port, uint8_t l4protocol) = 0;
// Addition for w/o SOME/IP SD End #########################################################
#endif
};

} // namespace sd
} // namespace vsomeip_v3

#endif // VSOMEIP_V3_SD_SERVICE_DISCOVERY_HPP_
