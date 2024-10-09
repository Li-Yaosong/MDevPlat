#ifndef ANDROIDDEBUGBRIDGE_H
#define ANDROIDDEBUGBRIDGE_H

#include <QObject>
#define adb AndroidDebugBridge::instance()
struct Device
{
    QString serialNum = "";
    QString status = "";
};
struct BatteryStatus
{
public:
    BatteryStatus() {}
    bool acPowered;
    bool usbPowered;
    bool wirelessPowered;
    int maxChargingCurrent;
    int maxChargingVoltage;
    int chargeCounter;
    int status;
    int health;
    bool present;
    int level;
    int scale;
    int voltage;
    int temperature;
    QString technology;
};
struct MemoryInfo {
    int total;            // MemTotal
    int free;             // MemFree
    int available;        // MemAvailable
    int buffers;          // Buffers
    int cached;           // Cached
    int swapTotal;        // SwapTotal
    int swapFree;         // SwapFree
    int active;           // Active
    int inactive;         // Inactive
    int activeAnon;       // Active(anon)
    int inactiveAnon;     // Inactive(anon)
    int activeFile;       // Active(file)
    int inactiveFile;     // Inactive(file)
    int unevictable;      // Unevictable
    int mlocked;          // Mlocked
    int dirty;            // Dirty
    int writeback;        // Writeback
    int anonPages;        // AnonPages
    int mapped;           // Mapped
    int shmem;            // Shmem
    int slab;             // Slab
    int sreclaimable;     // SReclaimable
    int sunreclaim;       // SUnreclaim
    int kernelStack;      // KernelStack
    int pageTables;       // PageTables
    int commitLimit;      // CommitLimit
    int committedAS;      // Committed_AS
    int vmallocTotal;     // VmallocTotal
    int vmallocUsed;      // VmallocUsed
    int vmallocChunk;     // VmallocChunk
    int cmaTotal;         // CmaTotal
    int cmaFree;          // CmaFree
    int ionTotalCache;    // IonTotalCache
    int ionTotalUsed;     // IonTotalUsed
    int pActiveAnon;      // PActive(anon)
    int pInactiveAnon;    // PInactive(anon)
    int pActiveFile;      // PActive(file)
    int pInactiveFile;    // PInactive(file)
    int isolate1Free;     // Isolate1Free
    int isolate2Free;     // Isolate2Free
    int rsvTotalUsed;     // RsvTotalUsed
};
struct NetworkCardInfo
{
public:
    QString name;
    QString linkEncap;
    QString driver;
    QString inetAddr;
    QString bcast;
    QString mask;
    QStringList inet6AddrSite;
    QStringList inet6AddrLink;
    QStringList inet6AddrGlobal;
    QString status;
    int mtu;
    int rxPackets;
    int txPackets;
    qint64 rxBytes;
    qint64 txBytes;
};
typedef QList<Device> DeviceList;
/**
 * @brief The Storage class 存储使用情况
 * @note 单位：bytes
 */
struct Storage
{
    Storage() {}
    int avail = 0;
    int used = 0;
    int size = 0;
    int use = 0;
};
class AndroidDebugBridge : public QObject
{
    Q_OBJECT
    //只读属性
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString adbPath READ adbPath CONSTANT)

public:
    static AndroidDebugBridge *instance();
    /**
     * @brief devices 获取设备列表
     * @return 返回设备序列号列表
     */
    DeviceList devices() const;
    /**
     * @brief deviceModel 获取设备型号
     * @param serialNum 设备序列号
     * @return 返回设备型号
     */
    QString deviceModel(const QString &serialNum) const;
    /**
     * @brief androidVersion 获取设备安卓版本
     * @param serialNum 设备序列号
     * @return 返回设备安卓版本
     * @note 例如：9
     */
    QString androidVersion(const QString &serialNum) const;
    /**
     * @brief cpuAbi 获取cpu架构
     * @param serialNum 设备序列号
     * @return 返回cpu架构
     * @note 例如：arm64-v8a
     */
    QString cpuAbi(const QString &serialNum) const;
    /**
     * @brief batteryStatus 获取电池状态
     * @param serialNum 设备序列号
     * @return 返回电池状态
     */
    BatteryStatus batteryStatus(const QString &serialNum) const;
    /**
     * @brief storage 获取存储使用情况
     * @param serialNum 设备序列号
     * @return 返回存储使用情况
     */
    Storage storage(const QString &serialNum) const;
    /**
     * @brief memory 获取内存使用情况
     * @param serialNum 设备序列号
     * @return 返回内存使用情况
     */
    Storage memory(const QString &serialNum) const;
    /**
     * @brief ethInfo 获取以太网信息
     * @param serialNum 设备序列号
     * @return 返回以太网信息
     */
    NetworkCardInfo ethInfo(const QString &serialNum) const;
    /**
     * @brief wlanInfo 获取无线网卡信息
     * @param serialNum 设备序列号
     * @return 返回无线网卡信息
     */
    NetworkCardInfo wlanInfo(const QString &serialNum) const;
    /**
     * @brief screenResolution 获取屏幕分辨率
     * @param serialNum 设备序列号
     * @return 返回屏幕分辨率
     */
    QSize screenResolution(const QString &serialNum) const;

    void startTcpIp(const QString &serialNum);

    /**
     * @brief path 获取adb路径
     * @return 返回adb路径
     */
    QString path() const;
    /**
     * @brief adbPath 获取adb可执行文件路径
     * @return 返回adb可执行文件路径
     */
    QString adbPath() const;
    void startAdbServer() const;
    void killAdbServer() const;
private:
    NetworkCardInfo parseNetworkCardInfo(const QString &serialNum, const QString &name) const;
    explicit AndroidDebugBridge(QObject *parent = nullptr);
    static AndroidDebugBridge *m_instance;
    QString m_path;
};

#endif // ANDROIDDEBUGBRIDGE_H
