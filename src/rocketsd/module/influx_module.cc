#include <rocketsd/module/influx_module.hh>

#include <QNetworkRequest>
#include <QUrl>

#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <log/log.hh>

namespace rocketsd::modules {

InfluxModule::InfluxModule(QObject* parent)
    : Module(parent)
    , network_(new QNetworkAccessManager(this))
    , lines_(0)
{
    connect(network_, &QNetworkAccessManager::finished, this, &InfluxModule::onError);
}

InfluxModule::~InfluxModule() {}

bool InfluxModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    if (!util::json::validate("InfluxModule", config,
        util::json::required(base_url_, "url"),
        util::json::required(max_lines_, "buffer_size")
    )) {
        return false;
    }

    if (!QUrl(base_url_.c_str()).isValid()) {
        logging::err("InfluxModule") << "Invalid URL" << logging::endl;
        return false;
    }

    logging::info("InfluxModule") << "Successfully init'd InfluxStation client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void InfluxModule::onMessage(Message message)
{
    std::stringstream local;
    local << message.source << " ";

    switch (message.measurement.value_case()) {
        case cute::proto::Measurement::kBool:
            local << (message.measurement.bool_() ? "true" : "false");
            break;
        case cute::proto::Measurement::kNumber:    
            local << message.measurement.number();
            break;
        case cute::proto::Measurement::kState:
            local << (message.measurement.state());
            break;
        case cute::proto::Measurement::kString:
            local << (message.measurement.string());
            break;
        case cute::proto::Measurement::VALUE_NOT_SET:
            logging::err("InfluxModule") << "Received invalid measurement, ignoring" << logging::endl;
            return;
    }
     
    local << " " << util::time::now<std::nano>() << std::endl;
    buffer_ << local.str();
    lines_++;

    if (lines_ >= max_lines_) {
        QNetworkRequest request(QUrl((base_url_ + "/write?db=anirniq").c_str()));
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-influx");
        network_->post(request, QByteArray(buffer_.str().c_str()));
        buffer_.str("");
        lines_ = 0;
    }
}

void InfluxModule::onError(QNetworkReply* reply)
{
    switch (reply->error()) {
        case QNetworkReply::NetworkError::NoError:
            break;

        default:
            logging::err("InfluxModule") << "HTTP error " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << logging::endl;
    }
}

} // namespace