#include "clientworker.h"
#include "tcpdatareceiver.h"
#include "simulator.h"
#include "globalsimulatormanager.h"
#include "handleldterminalconstants.h"

#include <QJsonDocument>
#include <QDebug>

using namespace HandleldTerminal;
using namespace Internal;

ClientWorker::ClientWorker(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    m_socket = socket;
    m_receiver = new TcpDataReceiver(m_socket.data(), this);
    m_receiver->registerHandler("LIST_SIMULATOR", this, &ClientWorker::listSimulator);
    m_receiver->registerHandler("ADD_SIMULATOR", this, &ClientWorker::addSimulator);
    m_receiver->registerHandler("UPDATE_SIMULATOR", this, &ClientWorker::updateSimulator);
    m_receiver->registerHandler("REMOVE_SIMULATOR", this, &ClientWorker::removeSimulator);
    m_receiver->registerHandler("START_SIMULATOR", this, &ClientWorker::startSimulator);
    m_receiver->registerHandler("STOP_SIMULATOR", this, &ClientWorker::stopSimulator);
}

void ClientWorker::listSimulator(const QVariantMap &map)
{
    Q_UNUSED(map);
    QList<Simulator *> all = qSimulatorManager->list();
    QVariantList resut;
    for (int i = 0; i < all.count(); ++i) {
        auto sim = all.at(i);
        QVariantMap info = sim->settings();
        info.insert("Name", sim->name());
        info.insert("Cpu", sim->abi());
        info.insert("Type", sim->type() == HandleldTerminal::Android ? "Android" : "Linux");
        info.insert("IsRunning", sim->isRunning());
        resut << info;
    }
    reply(resut);
}

void ClientWorker::addSimulator(const QVariantMap &map)
{
    QString simulatorData = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(simulatorData.toLocal8Bit());
    QVariantMap sim = doc.toVariant().toMap();
    QString t = value<QString>(sim, "Type");
    QString name = value<QString>(sim, "Name");
    HandleldTerminal::SimulatorType type = (t == "Android") ? HandleldTerminal::Android :
                                                              HandleldTerminal::Linux;
    QVariantMap ret;
    if (qSimulatorManager->exists(type, name)) {
        ret.insert("message", "The simulator is exists");
        ret.insert("result", "Error");
        reply(ret);
        return;
    }

    QVariantMap settings;
    settings.insert(HandleldTerminal::Constants::Simulator::NAME, name);
    settings.insert(HandleldTerminal::Constants::Simulator::ABI, sim.value("Cpu"));
    settings.insert(HandleldTerminal::Constants::Simulator::MEMORY, sim.value("Memory"));
    qSimulatorManager->create(type, settings);
    ret.insert("result", "Ok");
    reply(ret);
}

void ClientWorker::updateSimulator(const QVariantMap &map)
{
    QString simulatorData = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(simulatorData.toLocal8Bit());
    QVariantMap sim = doc.toVariant().toMap();
    QString t = value<QString>(sim, "Type");
    QString name = value<QString>(sim, "Name");
    HandleldTerminal::SimulatorType type = (t == "Android") ? HandleldTerminal::Android :
                                                              HandleldTerminal::Linux;
    QVariantMap ret;
    Simulator *simulator = qSimulatorManager->find(type, name);
    if (simulator == nullptr) {
        ret.insert("message", "The simulator is not exists");
        ret.insert("result", "Error");
        reply(ret);
        return;
    }

    QVariantMap settings;
    settings.insert(HandleldTerminal::Constants::Simulator::MEMORY, sim.value("Memory"));
    settings.insert(HandleldTerminal::Constants::Simulator::CPUNUMBER, sim.value("CpuNumber"));
    settings.insert(HandleldTerminal::Constants::Simulator::X_RESOLUTION, sim.value("X_Resolution"));
    settings.insert(HandleldTerminal::Constants::Simulator::Y_RESOLUTION, sim.value("Y_Resolution"));
    if (type == HandleldTerminal::Linux) {
        settings.insert(HandleldTerminal::Constants::Simulator::Linux::SSH_PORT, sim.value("Ssh_Port"));
        settings.insert(HandleldTerminal::Constants::Simulator::Linux::USERNAME, sim.value("User_Name"));
        settings.insert(HandleldTerminal::Constants::Simulator::Linux::PASSWORD, sim.value("Password"));
    }

    simulator->updateSettings(settings);
    QString messageReturn = tr("Update succeeded.");
    if (simulator->isRunning()) {
        messageReturn += tr(" The simulator is running now, and it will take effect after restarting.");
    }
    ret.insert("message", messageReturn);
    ret.insert("result", "Ok");
    reply(ret);
}

void ClientWorker::removeSimulator(const QVariantMap &map)
{
    QString simulatorData = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(simulatorData.toLocal8Bit());
    QVariantMap sim = doc.toVariant().toMap();
    QString t = value<QString>(sim, "Type");
    QString name = value<QString>(sim, "Name");
    HandleldTerminal::SimulatorType type = (t == "Android") ? HandleldTerminal::Android :
                                                              HandleldTerminal::Linux;
    QVariantMap ret;
    Simulator *simulator = qSimulatorManager->find(type, name);
    if (simulator == nullptr) {
        ret.insert("message", "Unable to find emulator.");
        ret.insert("result", "Error");
        reply(ret);
        return;
    }
    qSimulatorManager->remove(simulator);
    ret.insert("result", "Ok");
    reply(ret);
}

void ClientWorker::startSimulator(const QVariantMap &map)
{
    QString simulatorData = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(simulatorData.toLocal8Bit());
    QVariantMap sim = doc.toVariant().toMap();
    QString t = value<QString>(sim, "Type");
    QString name = value<QString>(sim, "Name");
    HandleldTerminal::SimulatorType type = (t == "Android") ? HandleldTerminal::Android :
                                                              HandleldTerminal::Linux;

    QVariantMap ret;
    Simulator *simulator = qSimulatorManager->find(type, name);
    if (simulator == nullptr) {
        ret.insert("message", "Unable to find emulator.");
        ret.insert("result", "Error");
        reply(ret);
        return;
    }

    if (simulator->isRunning()) {
        ret.insert("message", "The simulator is already running.");
        ret.insert("result", "Ok");
        reply(ret);
        return;
    }
    bool result = simulator->start();
    ret.insert("message", QString("The simulator is start %1.").arg(result ? "succeeded" : "failed"));
    ret.insert("result", result ? "Ok" : "Error");
    reply(ret);
}

void ClientWorker::stopSimulator(const QVariantMap &map)
{
    QString simulatorData = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(simulatorData.toLocal8Bit());
    QVariantMap sim = doc.toVariant().toMap();
    QString t = value<QString>(sim, "Type");
    QString name = value<QString>(sim, "Name");
    HandleldTerminal::SimulatorType type = (t == "Android") ? HandleldTerminal::Android :
                                                              HandleldTerminal::Linux;

    QVariantMap ret;
    Simulator *simulator = qSimulatorManager->find(type, name);
    if (simulator == nullptr) {
        ret.insert("message", "Unable to find emulator.");
        ret.insert("result", "Error");
        reply(ret);
        return;
    }

    if (!simulator->isRunning()) {
        ret.insert("message", "The simulator is already stopped.");
        ret.insert("result", "Ok");
        reply(ret);
        return;
    }
    bool result = simulator->stop();
    ret.insert("message", QString("The simulator is stop %1.").arg(result ? "succeeded" : "failed"));
    ret.insert("result", result ? "Ok" : "Error");
    reply(ret);
}

void ClientWorker::reply(const QVariant &result)
{
    QJsonDocument doc = QJsonDocument::fromVariant(result);
    QByteArray r = doc.toJson();
    m_socket->write(r);
}

