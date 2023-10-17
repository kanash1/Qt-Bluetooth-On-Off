#include "bluetoothworker.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Radios;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Enumeration;

BluetoothWorker::BluetoothWorker()
	: radio(nullptr)
{
}

void BluetoothWorker::init() {
	int state = 0;
	radio = BluetoothAdapter::GetDefaultAsync().get().GetRadioAsync().get();
	if (radio.State() == RadioState::On)
		state = 1;
	else if (radio.State() == RadioState::Off)
		state = 0;
	emit initFinished(state);
}

winrt::fire_and_forget  BluetoothWorker::setRadioPowerState(int state) {
	if (radio != nullptr) {
		co_await radio.SetStateAsync((state == 1) ? RadioState::On : RadioState::Off);
		emit radioPowerStateChanged();
	}
}

void BluetoothWorker::startDiscovery()
{
	// Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
	auto requestedProperties = single_threaded_vector<hstring>({ L"System.Devices.Aep.DeviceAddress" });

	// BT_Code: Example showing paired and non-paired in a single query.
	hstring aqsAllBluetoothDevices = L"(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\" OR System.Devices.Aep.ProtocolId:=\"{E0CBF06C-CD8B-4647-BB8A-263B43F0F974}\")";

	deviceWatcher =	Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(
			aqsAllBluetoothDevices,
			requestedProperties,
			DeviceInformationKind::AssociationEndpoint);
	// Register event handlers before starting the watcher.
	deviceWatcherAddedToken = deviceWatcher.Added({ this, &BluetoothWorker::DeviceWatcher_Added });
	deviceWatcherUpdatedToken = deviceWatcher.Updated({ this, &BluetoothWorker::DeviceWatcher_Updated });
	deviceWatcherRemovedToken = deviceWatcher.Removed({ this, &BluetoothWorker::DeviceWatcher_Removed });
	deviceWatcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ this, &BluetoothWorker::DeviceWatcher_EnumerationCompleted });
	deviceWatcherStoppedToken = deviceWatcher.Stopped({ this, &BluetoothWorker::DeviceWatcher_Stopped });

	deviceWatcher.Start();
}

int BluetoothWorker::FindBluetoothDeviceDisplay(winrt::hstring id)
{
	int count = deviceInformationVector.size();
	for (int index = 0; index < count; index++)
	{
		if (deviceInformationVector.at(index).Id() == id)
			return index;
	}
	return -1;
}

int BluetoothWorker::FindUnknownDevices(winrt::hstring id)
{
	int count = UnknownDevices.size();
	for (int index = 0; index < count; index++)
	{
		if (UnknownDevices.at(index).Id() == id)
			return index;
	}
	return -1;
}

void BluetoothWorker::DeviceWatcher_Added(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformation deviceInfo)
{
	if (sender == deviceWatcher) {
		// Make sure device isn't already present in the list.
		int index = FindBluetoothDeviceDisplay(deviceInfo.Id());
		if (index < 0) {
			if (!deviceInfo.Name().empty())	{
				// If device has a friendly name display it immediately.
				m_knownDevices.Append(deviceInfo);
				deviceInformationVector.push_back(deviceInfo);
				emit eventFromDiscovery(0, index, QString::fromStdString(winrt::to_string(deviceInfo.Id())), QString::fromStdString(winrt::to_string(deviceInfo.Name())));
			} else {
				// Add it to a list in case the name gets updated later. 
				UnknownDevices.push_back(deviceInfo);
			}
		}
	}
}

void BluetoothWorker::DeviceWatcher_Updated(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate)
{
	if (sender == deviceWatcher) {
		int index = FindBluetoothDeviceDisplay(deviceInfoUpdate.Id());
		if (index >= 0) {
			// Device is already being displayed - update UX.
			deviceInformationVector.at(index).Update(deviceInfoUpdate);
			emit eventFromDiscovery(1, index, QString::fromStdString(winrt::to_string(deviceInfoUpdate.Id())), QString::fromStdString(winrt::to_string(deviceInformationVector.at(index).Name())));
		}
		index = FindUnknownDevices(deviceInfoUpdate.Id());
		if (index >= 0) {
			UnknownDevices.at(index).Update(deviceInfoUpdate);
			if (!UnknownDevices.at(index).Name().empty()) {
				m_knownDevices.Append(UnknownDevices.at(index));
				deviceInformationVector.push_back(UnknownDevices.at(index));
				emit eventFromDiscovery(0, index, QString::fromStdString(winrt::to_string(UnknownDevices.at(index).Id())), QString::fromStdString(winrt::to_string(UnknownDevices.at(index).Name())));
			}
		}
	}
}

void BluetoothWorker::DeviceWatcher_Removed(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate)
{
	if (sender == deviceWatcher) {
		// Find the corresponding DeviceInformation in the collection and remove it.
		int index = FindBluetoothDeviceDisplay(deviceInfoUpdate.Id());
		if (index >= 0) {
			m_knownDevices.RemoveAt(index);
			deviceInformationVector.erase(deviceInformationVector.begin() + index);
			emit eventFromDiscovery(2, index, QString::fromStdString(winrt::to_string(deviceInfoUpdate.Id())), "");
		}
		index = FindUnknownDevices(deviceInfoUpdate.Id());
		if (index >= 0) {
			UnknownDevices.erase(UnknownDevices.begin() + index);
		}
	}
}

void BluetoothWorker::DeviceWatcher_EnumerationCompleted(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&)
{
	if (sender == deviceWatcher) {
		QString addText = QString::number(m_knownDevices.Size()) + " devices found. Enumeration completed.";
		emit eventFromDiscovery(3, 0, addText, "");
	}
}

void BluetoothWorker::DeviceWatcher_Stopped(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&)
{
	if (sender == deviceWatcher)
	{
		QString addText = "Stopped ";
		emit eventFromDiscovery(3, 0, addText, "");
	}
}

/// Stops watching for all nearby Bluetooth devices.
void BluetoothWorker::StopDeviceWatcher()
{
	if (deviceWatcher != nullptr)
	{
		// Unregister the event handlers.
		deviceWatcher.Added(deviceWatcherAddedToken);
		deviceWatcher.Updated(deviceWatcherUpdatedToken);
		deviceWatcher.Removed(deviceWatcherRemovedToken);
		deviceWatcher.EnumerationCompleted(deviceWatcherEnumerationCompletedToken);
		deviceWatcher.Stopped(deviceWatcherStoppedToken);

		// Stop the watcher.
		deviceWatcher.Stop();
		deviceWatcher = nullptr;
	}
}

void BluetoothWorker::PairingRequestedHandler(DeviceInformationCustomPairing sender, DevicePairingRequestedEventArgs args)
{
	args.Accept();
}

winrt::fire_and_forget BluetoothWorker::pairingDevice(int index)
{
	int iii = index;
	QString devName = QString::fromStdString(winrt::to_string(deviceInformationVector.at(index).Name()));

	DeviceInformationCustomPairing customPairing = deviceInformationVector.at(index).Pairing().Custom();
	DevicePairingKinds ceremonySelection = DevicePairingKinds::None | DevicePairingKinds::ConfirmOnly;

	event_token handlerPairingRequestedToken = customPairing.PairingRequested({ this, &BluetoothWorker::PairingRequestedHandler });
//	winrt::Windows::Foundation::IAsyncOperation<DevicePairingResult> pairingOperation = customPairing.PairAsync(ceremonySelection, DevicePairingProtectionLevel::None);
	
	/*
	winrt::Windows::Foundation::AsyncStatus pairStatus = pairingOperation.wait_for(winrt::Windows::Foundation::TimeSpan(PAIR_TIMEOUT));
	if (pairStatus != AsyncStatus::Completed) {
		if (pairStatus == AsyncStatus::Started) {
			pairingOperation.Cancel();
		}
		return false;
	}
*/
	// BT_Code: Pair the currently selected device.
	DevicePairingResult result = co_await customPairing.PairAsync(ceremonySelection, DevicePairingProtectionLevel::None);




	//DevicePairingResult result = co_await deviceInformationVector.at(index).Pairing().PairAsync();
//	DevicePairingResult result = pairingOperation.GetResults();
	DevicePairingResultStatus status = result.Status();
	switch (status) {
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::Paired:
		//emit eventFromDiscovery(4, 0, "Paired", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::NotReadyToPair:
		emit eventFromDiscovery(4, 0, "NotReadyToPair", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::NotPaired:
		emit eventFromDiscovery(4, 0, "NotPaired", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::AlreadyPaired:
		emit eventFromDiscovery(4, 0, "AlreadyPaired", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::ConnectionRejected:
		emit eventFromDiscovery(4, 0, "ConnectionRejected", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::TooManyConnections:
		emit eventFromDiscovery(4, 0, "TooManyConnections", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::HardwareFailure:
		emit eventFromDiscovery(4, 0, "HardwareFailure", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::AuthenticationTimeout:
		emit eventFromDiscovery(4, 0, "AuthenticationTimeout", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::AuthenticationNotAllowed:
		emit eventFromDiscovery(4, 0, "AuthenticationNotAllowed", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::AuthenticationFailure:
		emit eventFromDiscovery(4, 0, "AuthenticationFailure", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::NoSupportedProfiles:
		emit eventFromDiscovery(4, 0, "NoSupportedProfiles", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::ProtectionLevelCouldNotBeMet:
		emit eventFromDiscovery(4, 0, "ProtectionLevelCouldNotBeMet", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::AccessDenied:
		emit eventFromDiscovery(4, 0, "AccessDenied", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::InvalidCeremonyData:
		emit eventFromDiscovery(4, 0, "InvalidCeremonyData", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::PairingCanceled:
		emit eventFromDiscovery(4, 0, "PairingCanceled", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::OperationAlreadyInProgress:
		emit eventFromDiscovery(4, 0, "OperationAlreadyInProgress", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::RequiredHandlerNotRegistered:
		emit eventFromDiscovery(4, 0, "RequiredHandlerNotRegistered", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::RejectedByHandler:
		emit eventFromDiscovery(4, 0, "RejectedByHandler", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::RemoteDeviceHasAssociation:
		emit eventFromDiscovery(4, 0, "RemoteDeviceHasAssociation", "");
		break;
	case winrt::Windows::Devices::Enumeration::DevicePairingResultStatus::Failed:
		emit eventFromDiscovery(4, 0, "Failed", "");
		break;
	default:
		break;
	}
	
}

winrt::fire_and_forget BluetoothWorker::unPairingDevice(int index)
{
	/*
	// Gray out the unpair button and results view while unpairing is in progress.
	resultsListView->IsEnabled = false;
	unpairButton->IsEnabled = false;
	rootPage->NotifyUser("Unpairing started. Please wait...", NotifyType::StatusMessage);
	*/

	DeviceUnpairingResult unpairingResult = co_await deviceInformationVector.at(index).Pairing().UnpairAsync();
	DeviceUnpairingResultStatus status = unpairingResult.Status();
	if (status == DeviceUnpairingResultStatus::Unpaired) {
		emit eventFromDiscovery(4, 0, "Unpaired", "");
	} else {
		emit eventFromDiscovery(4, 0, "Failed", "");
	}
}

BluetoothWorker::~BluetoothWorker()
{
	
}
/*

Pairing a Microsoft Windows App

I am trying to get a Microsoft Windows App to pair with a CYBLE-416045-02 module using pin confirmation. I can successfully pair with the module using CySmart, so I am pretty sure that I have the basics correct on the module side. But my windows app goes off the rails. Of course, one of the big differences is that CySmart goes through its custom BLE dongle, while the Windows app tries to use the PCs bluethooth system.

In the Windows app, I followed the guidance at Pair devices - UWP applications | Microsoft Docs and did the following:

DevicePairingKinds pk = DevicePairingKinds.ConfirmPinMatch;

  DeviceInformation deviceInfo = btdev.DeviceInformation;
  DeviceInformationPairing devicePairing = deviceInfo.Pairing;
  DeviceInformationCustomPairing custom = devicePairing.Custom;

  custom.PairingRequested += pairhandle;

  DevicePairingResult result = await custom.PairAsync(pk);

  custom.PairingRequested -= pairhandle;
  */