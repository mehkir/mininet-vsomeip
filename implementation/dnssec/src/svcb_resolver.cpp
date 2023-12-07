#include "../include/svcb_resolver.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include "../../timestamps/include/timestamp_collector.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <string>
#include <iomanip>

namespace vsomeip_v3 {
    svcb_resolver::svcb_resolver() : dns_resolver_(dns_resolver::get_instance()) {
        dns_resolver_->initialize(DNS_SERVER_IP);
    }

    svcb_resolver::~svcb_resolver() {
    }

    void service_svcb_resolve_callback(void* _data, int _status, int _timeouts,
                unsigned char* _abuf, int _alen) {
        LOG_DEBUG(__func__ << " is called")
        service_data_and_cbs* servicedata_and_cbs = reinterpret_cast<service_data_and_cbs*>(_data);
        
        if (_status) {
            std::cerr << __func__ << " Bad DNS response" << std::endl;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            return;
        }

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        svcb_reply* svcbreply;
        if ((parse_svcb_reply(copy, _alen, &svcbreply)) != ARES_SUCCESS) {
            std::cout << "Parsing SVCB reply failed" << std::endl;
            delete_svcb_reply(svcbreply);
            return;
        }
        
        svcb_reply* svcb_reply_ptr = svcbreply;
        while (svcb_reply_ptr != nullptr) {
            //std::cout << "svcb_resolver\n" << *svcb_reply_ptr << std::endl;
            std::string reliable_address = "", unreliable_address = "";
            // uint16_t reliable_port = 0, unreliable_port = 0;
            int l4protocol = std::stoi(svcb_reply_ptr->get_svcb_key(L4PROTOCOL));
            switch (l4protocol)
            {
            case IPPROTO_UDP:
                unreliable_address = svcb_reply_ptr->ipv4_address_string_;
                // unreliable_port = svcb_reply_ptr->port_;
                servicedata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            case IPPROTO_TCP:
                reliable_address = svcb_reply_ptr->ipv4_address_string_;
                // reliable_port = svcb_reply_ptr->port_;
                servicedata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            default:
                break;
            }
            //Fill with concrete values in case instance, major and minor were not specified
            servicedata_and_cbs->instance_ = (vsomeip_v3::instance_t) std::stoi(svcb_reply_ptr->get_svcb_key(INSTANCE),0,16);
            servicedata_and_cbs->major_ = (vsomeip_v3::major_version_t) std::stoi(svcb_reply_ptr->get_svcb_key(MAJOR_VERSION),0,16);
            servicedata_and_cbs->minor_ = (vsomeip_v3::minor_version_t) std::stoi(svcb_reply_ptr->get_svcb_key(MINOR_VERSION),0,16);
            servicedata_and_cbs->add_service_svcb_entry_cache_callback_(servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->major_, servicedata_and_cbs->minor_, l4protocol, servicedata_and_cbs->ipv4_address_, svcb_reply_ptr->port_);
            servicedata_and_cbs->verify_service_info_callback_(servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->major_, servicedata_and_cbs->minor_);
            //Assemble TLSA QNAME
            std::stringstream tlsa_request;
            tlsa_request << ATTRLEAFBRANCH;
            tlsa_request << "minor" << svcb_reply_ptr->get_svcb_key(MINOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "major" << svcb_reply_ptr->get_svcb_key(MAJOR_VERSION);
            tlsa_request << ".";
            tlsa_request << "instance" << svcb_reply_ptr->get_svcb_key(INSTANCE);
            tlsa_request << ".";
            tlsa_request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << servicedata_and_cbs->service_;
            tlsa_request << ".";
            tlsa_request << SERVICE_PARENTDOMAIN;
            servicedata_and_cbs->request_service_tlsa_record_callback_(servicedata_and_cbs, tlsa_request.str());

            svcb_reply_ptr = svcb_reply_ptr->svcb_reply_next_;
        }
        delete_svcb_reply(svcbreply);
        
        //delete servicedata_and_cbs;
        delete[] copy;
    }

    void svcb_resolver::request_service_svcb_record(service_data_and_cbs* _service_data_and_cbs) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (_service_data_and_cbs->minor_ != ANY_MINOR) {
            request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << _service_data_and_cbs->minor_;
            request << ".";
        }
        if (_service_data_and_cbs->major_ != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << _service_data_and_cbs->major_;
            request << ".";
        }
        if (_service_data_and_cbs->instance_ != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << _service_data_and_cbs->instance_;
            request << ".";
        }
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << _service_data_and_cbs->service_;
        request << ".";
        request << SERVICE_PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, service_svcb_resolve_callback, _service_data_and_cbs);
    }
} /* end namespace vsomeip_v3 */