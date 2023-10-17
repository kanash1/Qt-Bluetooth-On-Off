#pragma once

#include <QThread>
#include <QTextBrowser>
#include <QListWidget>
#include <QLabel>

class BluetoothManager : public QObject {
	Q_OBJECT
public:
	BluetoothManager();
	~BluetoothManager();
	void setRadioPowerState(int state);
	int getRadioPowerState() const;
	void startBluetoothDiscovery();
	void pairing(int index);
	void unPairing(int index);

public:
	QTextBrowser* m_textBrowser;
	QListWidget*  m_devicesListWidget;
	QLabel*       m_pairingLabel;

signals:
	void initFinished();
	void radioPowerStateSetRequested(int state);
	void radioPowerStateChanged();
	void startDiscovery();
	void pairingDevice(int index);
	void unPairingDevice(int index);

private slots:
	void setInitialRadioPowerState(int state);
	void eventFromDiscovery(int operation, int index, QString id, QString name); // operation 0 - добавить 1 - изменить 2 - удалить

private:
	QThread workerThread;
	int radioPowerState;

};









	//winrt::Windows::Devices::Radios::Radio* initilizeRadio();
	//void initilizeRadio(std::promise<winrt::Windows::Devices::Radios::Radio>&& p);

	//enum class PowerState {
	//	On,				// The radio is powered on.
	//	Off				// The radio is powered off.
	//};

	//winrt::Windows::Foundation::IAsyncAction TryGetRadio(winrt::Windows::Devices::Radios::Radio& radio);
	//winrt::Windows::Foundation::IAsyncAction TryChangeState(winrt::Windows::Devices::Radios::Radio& radio, PowerState state);

	//void cringe(winrt::Windows::Devices::Radios::Radio& radio, const PowerState& state);

	/*template<class Container>
	class IBluetoothDeviceDiscoveryAgent {
	public:
		virtual void start() = 0;
		virtual void stop() = 0;
		virtual Container getDiscoveredDevices() const = 0;
	};

	template<class DeviceDiscoveryAgent>
	class IBluetoothManager {
	public:
		virtual void setPowerState(const PowerState& state) = 0;
		virtual PowerState getPowerState() = 0;
		virtual DeviceDiscoveryAgent getDiscoveryAgent() const = 0;
	};*/

	// TODO
	//class WinBluetoothDeviceDiscoveryAgent : public IBluetoothDeviceDiscoveryAgent<std::list<winrt::Windows::Devices::Enumeration::DeviceInformation>> {
	//public:
	//	WinBluetoothDeviceDiscoveryAgent(QListWidget* listWidget = nullptr);
	//	~WinBluetoothDeviceDiscoveryAgent();
	//	void start() override;
	//	// TODO
	//	void stop() override;
	//	// TODO
	//	std::list<winrt::Windows::Devices::Enumeration::DeviceInformation> getDiscoveredDevices() const override;
	//private:
	//	void device_Added(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformation deviceInfo);
	//	void device_Updated(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	//	// TODO
	//	void device_Removed(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	//	// TODO
	//	void device_EnumerationCompleted(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);
	//	// TODO
	//	void device_Stopped(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);

	//	winrt::hstring deviceProperties;
	//	winrt::Windows::Devices::Enumeration::DeviceWatcher deviceWatcher;
	//	std::list<winrt::Windows::Devices::Enumeration::DeviceInformation> devices;
	//	winrt::event_token deviceWatcherAddedToken;
	//	winrt::event_token deviceWatcherUpdatedToken;
	//	winrt::event_token deviceWatcherRemovedToken;
	//	winrt::event_token deviceWatcherEnumerationCompletedToken;
	//	winrt::event_token deviceWatcherStoppedToken;
	//	QListWidget* listWidget;
	//};

	/*class WinBluetoothManager : public IBluetoothManager<WinBluetoothDeviceDiscoveryAgent> {
	public:
		WinBluetoothManager(QListWidget* listWidget = nullptr, QPushButton* button = nullptr);
		void setPowerState(const PowerState& state) override;
		PowerState getPowerState() override;
		WinBluetoothDeviceDiscoveryAgent getDiscoveryAgent() const override;
	private:
		void initilizeRadio();
		void setPowerStateWinRT(const PowerState& state);

		QListWidget* listWidget;
		winrt::Windows::Devices::Radios::Radio radio;
		WinBluetoothDeviceDiscoveryAgent discoveryAgent;
		PowerState state;
		
	};*/

	//class Worker : public QObject
	//{
	//	Q_OBJECT

	//public slots:
	//	void setState(int) {
	//		using winrt::Windows::Devices::Radios::RadioState;
	//		using winrt::Windows::Devices::Radios::Radio;
	//		winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync().get().GetRadioAsync().get().SetStateAsync(winrt::Windows::Devices::Radios::RadioState::On).get();
	//		emit radioSettedState(6);
	//	}

	//signals:
	//	void radioSettedState(int);
	//};

