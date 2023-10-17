#pragma once

// https://github.com/microsoft/Windows.UI.Composition-Win32-Samples/issues/47
#include "winrt/base.h"
namespace winrt::impl
{
	template <typename Async>
	auto wait_for(Async const& async, Windows::Foundation::TimeSpan const& timeout);
}

#include <QObject>
#include <QThread>
#include <QTextBrowser>
#include <winrt/Windows.Foundation.h>
#include "winrt/Windows.Foundation.Collections.h"
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Enumeration.h>

#pragma comment(lib, "WindowsApp.lib")

class States {
	Q_GADGET
public:
	States() = delete;
	enum class RadioPower {
		Unknown,
		On,
		Off
	};
	Q_ENUM(RadioPower)
};

class BluetoothWorker : public QObject
{
	Q_OBJECT

public:
	BluetoothWorker();
	~BluetoothWorker();
public slots:
	void init();
	winrt::fire_and_forget setRadioPowerState(int state);
	void startDiscovery();
//	void pairingDevice(int index);
	winrt::fire_and_forget pairingDevice(int index);
	winrt::fire_and_forget unPairingDevice(int index);

signals:
	void initFinished(int state);
	void radioPowerStateChanged();
	void eventFromDiscovery(int operation, int index, QString id, QString name); // operation 0 - добавить 1 - изменить 2 - удалить 3 - сообщение

private:
	void StopDeviceWatcher();
	int FindBluetoothDeviceDisplay(winrt::hstring id);
	int FindUnknownDevices(winrt::hstring id);

	void DeviceWatcher_Added(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformation deviceInfo);
	void DeviceWatcher_Updated(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	void DeviceWatcher_Removed(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
	void DeviceWatcher_EnumerationCompleted(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);
	void DeviceWatcher_Stopped(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&);

	void PairingRequestedHandler(winrt::Windows::Devices::Enumeration::DeviceInformationCustomPairing sender, winrt::Windows::Devices::Enumeration::DevicePairingRequestedEventArgs args);

private:
	std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> deviceInformationVector;
	std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation> UnknownDevices;

	winrt::Windows::Devices::Radios::Radio radio;

	winrt::Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_knownDevices = winrt::single_threaded_observable_vector<winrt::Windows::Foundation::IInspectable>();

	winrt::event_token deviceWatcherAddedToken;
	winrt::event_token deviceWatcherUpdatedToken;
	winrt::event_token deviceWatcherRemovedToken;
	winrt::event_token deviceWatcherEnumerationCompletedToken;
	winrt::event_token deviceWatcherStoppedToken;

//	winrt::event_token handlerPairingRequestedToken;

};
