#include "../include/svcb_resolver.hpp"
#include "../include/logger.hpp"
#include "../include/parse_svcb_reply.hpp"
#include "../../timestamps/include/timestamp_collector.hpp"
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <string>
#include <iomanip>
#include <boost/algorithm/string.hpp>

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
            delete servicedata_and_cbs;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            delete servicedata_and_cbs;
            return;
        }

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        svcb_reply* svcbreply;
        if ((parse_svcb_reply(copy, _alen, &svcbreply)) != ARES_SUCCESS) {
            std::cout << "Parsing service SVCB reply failed" << std::endl;
            delete servicedata_and_cbs;
            delete[] copy;
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
            servicedata_and_cbs->validate_offer_callback_(servicedata_and_cbs->service_, servicedata_and_cbs->instance_, servicedata_and_cbs->major_, servicedata_and_cbs->minor_);
            servicedata_and_cbs->request_service_tlsa_record_callback_(servicedata_and_cbs);

            svcb_reply_ptr = svcb_reply_ptr->svcb_reply_next_;
        }
        delete[] copy;
        delete_svcb_reply(svcbreply);
    }

    void client_svcb_resolve_callback(void* _data, int _status, int _timeouts,
                unsigned char* _abuf, int _alen) {
        LOG_DEBUG(__func__ << " is called")
        client_data_and_cbs* clientdata_and_cbs = reinterpret_cast<client_data_and_cbs*>(_data);
        
        if (_status) {
            std::cerr << __func__ << " Bad DNS response" << std::endl;
            delete clientdata_and_cbs;
            return;
        }

        if (_timeouts) {
            std::cerr << __func__ << " DNS request timeout" << std::endl;
            delete clientdata_and_cbs;
            return;
        }

        unsigned char* copy = new unsigned char[_alen];
        memcpy(copy, _abuf, _alen);
        svcb_reply* svcbreply;
        if ((parse_svcb_reply(copy, _alen, &svcbreply)) != ARES_SUCCESS) {
            std::cout << "Parsing client SVCB reply failed" << std::endl;
            delete clientdata_and_cbs;
            delete[] copy;
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
                clientdata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            case IPPROTO_TCP:
                reliable_address = svcb_reply_ptr->ipv4_address_string_;
                // reliable_port = svcb_reply_ptr->port_;
                clientdata_and_cbs->ipv4_address_ = boost::asio::ip::address_v4::from_string(unreliable_address);
                break;
            default:
                break;
            }
            std::set<std::string> str_ports;
            boost::split(str_ports, svcb_reply_ptr->get_svcb_key(CLIENT_PORTS), boost::is_any_of(","));
            std::set<port_t> ports;
            for (auto port : str_ports) { ports.insert((port_t) std::stoi(port)); }
            //Fill with concrete values in case instance and major were not specified
            clientdata_and_cbs->instance_ = (vsomeip_v3::instance_t) std::stoi(svcb_reply_ptr->get_svcb_key(INSTANCE),0,16);
            clientdata_and_cbs->major_ = (vsomeip_v3::major_version_t) std::stoi(svcb_reply_ptr->get_svcb_key(MAJOR_VERSION),0,16);
            clientdata_and_cbs->add_client_svcb_entry_cache_callback_(clientdata_and_cbs->client_, clientdata_and_cbs->service_, clientdata_and_cbs->instance_, clientdata_and_cbs->major_, l4protocol, clientdata_and_cbs->ipv4_address_, ports);
            clientdata_and_cbs->request_client_tlsa_record_callback_(clientdata_and_cbs);

            svcb_reply_ptr = svcb_reply_ptr->svcb_reply_next_;
        }
        delete[] copy;
        delete_svcb_reply(svcbreply);
    }

    void svcb_resolver::request_service_svcb_record(service_data_and_cbs* _service_data_and_cbs) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (_service_data_and_cbs->minor_ != ANY_MINOR) {
            request << "minor0x" << std::hex << std::setw(8) << std::setfill('0') << (int) _service_data_and_cbs->minor_;
            request << ".";
        }
        if (_service_data_and_cbs->major_ != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << (int) _service_data_and_cbs->major_;
            request << ".";
        }
        if (_service_data_and_cbs->instance_ != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << (int) _service_data_and_cbs->instance_;
            request << ".";
        }
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << (int) _service_data_and_cbs->service_;
        request << ".";
        request << SERVICE_PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, service_svcb_resolve_callback, _service_data_and_cbs);
    }

    void svcb_resolver::request_client_svcb_record(client_data_and_cbs* _client_data_and_cbs) {
        std::stringstream request;
        request << ATTRLEAFBRANCH;
        if (_client_data_and_cbs->major_ != ANY_MAJOR) {
            request << "major0x" << std::hex << std::setw(2) << std::setfill('0') << (int) _client_data_and_cbs->major_;
            request << ".";
        }
        if (_client_data_and_cbs->instance_ != ANY_INSTANCE) {
            request << "instance0x" << std::hex << std::setw(4) << std::setfill('0') << (int) _client_data_and_cbs->instance_;
            request << ".";
        }
        request << "service0x" << std::hex << std::setw(4) << std::setfill('0') << (int) _client_data_and_cbs->service_;
        request << ".";
        request << "id0x" << std::hex << std::setw(4) << std::setfill('0') << (int) _client_data_and_cbs->client_;
        request << ".";
        request << CLIENT_PARENTDOMAIN;
        dns_resolver_->resolve(request.str().c_str(), C_IN, T_SVCB, client_svcb_resolve_callback, _client_data_and_cbs);
    }
} /* end namespace vsomeip_v3 */