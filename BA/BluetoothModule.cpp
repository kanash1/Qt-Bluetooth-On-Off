#include "BluetoothModule.h"
#include "bluetoothworker.h"

	BluetoothManager::BluetoothManager() : radioPowerState(0)
	{
		BluetoothWorker* worker = new BluetoothWorker();
		worker->moveToThread(&workerThread);
		//qRegisterMetaType<States::RadioPower>("States::RadioPower");
		connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
		connect(&workerThread, &QThread::started, worker, &BluetoothWorker::init);
		connect(worker, &BluetoothWorker::initFinished, this, &BluetoothManager::setInitialRadioPowerState);
		connect(this, &BluetoothManager::radioPowerStateSetRequested, this, [=](int state) { this->radioPowerState = state; });
		connect(this, &BluetoothManager::radioPowerStateSetRequested, worker, &BluetoothWorker::setRadioPowerState);
		connect(worker, &BluetoothWorker::radioPowerStateChanged, this, &BluetoothManager::radioPowerStateChanged);
		connect(worker, &BluetoothWorker::eventFromDiscovery, this, &BluetoothManager::eventFromDiscovery);
		connect(this, &BluetoothManager::startDiscovery, worker, &BluetoothWorker::startDiscovery);
		connect(this, &BluetoothManager::pairingDevice, worker, &BluetoothWorker::pairingDevice);
		connect(this, &BluetoothManager::unPairingDevice, worker, &BluetoothWorker::unPairingDevice);

		workerThread.start();
	}

	void BluetoothManager::startBluetoothDiscovery()
	{
		emit startDiscovery();
	}

	void BluetoothManager::pairing(int index)
	{
		emit pairingDevice(index);
	}

	void BluetoothManager::unPairing(int index)
	{
		emit unPairingDevice(index);
	}

	void BluetoothManager::eventFromDiscovery(int operation, int index, QString id, QString name) {
		switch (operation) {
		case 0:
			m_devicesListWidget->addItem(name);
			m_textBrowser->append("Added " + id + " " + name);
			break;
		case 1: m_textBrowser->append("Updated " + id);
			m_devicesListWidget->item(index)->setText(name);
			break;
		case 2:
			m_devicesListWidget->takeItem(index);
			m_textBrowser->append("Removed " + id + "row " + QString::number(index));
			break;
		case 3:
			m_textBrowser->append(id);
			break;
		case 4:
			m_pairingLabel->setText(id);
			break;
		}
		
	}

	void BluetoothManager::setRadioPowerState(int state) {
		emit radioPowerStateSetRequested(state);
	}
	
	int BluetoothManager::getRadioPowerState() const {
		return radioPowerState;
	}

	void BluetoothManager::setInitialRadioPowerState(int state) {
		radioPowerState = state;
		emit initFinished();
	}

	BluetoothManager::~BluetoothManager() {
		workerThread.quit();
		workerThread.wait();
	}






	/*-------------------OLD UNUSED FUNCTIONS AND METHODS-------------------*/

	//// TODO
	//WinBluetoothDeviceDiscoveryAgent::WinBluetoothDeviceDiscoveryAgent(QListWidget* listWidget) :
	//	deviceProperties(L"(System.Devices.DevObjectType:=5 AND "
	//					"(System.Devices.Aep.ProtocolId:=\"{BB7BB05E-5972-42B5-94FC-76EAA7084D49}\" OR "
	//					"System.Devices.Aep.ProtocolId:=\"{E0CBF06C-CD8B-4647-BB8A-263B43F0F974}\"))"),
	//	deviceWatcher(winrt::Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(deviceProperties)),
	//	listWidget(listWidget)
	//{
	//	deviceWatcherAddedToken = deviceWatcher.Added({ this, &WinBluetoothDeviceDiscoveryAgent::device_Added });
	//	deviceWatcherUpdatedToken = deviceWatcher.Updated({ this, &WinBluetoothDeviceDiscoveryAgent::device_Updated });
	//	deviceWatcherRemovedToken = deviceWatcher.Removed({ this, &WinBluetoothDeviceDiscoveryAgent::device_Removed });
	//	deviceWatcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ this, &WinBluetoothDeviceDiscoveryAgent::device_EnumerationCompleted });
	//	deviceWatcherStoppedToken = deviceWatcher.Stopped({ this, &WinBluetoothDeviceDiscoveryAgent::device_Stopped });
	//}

	//WinBluetoothDeviceDiscoveryAgent::~WinBluetoothDeviceDiscoveryAgent() {
	//	if (deviceWatcher != nullptr) {
	//		deviceWatcher.Added(deviceWatcherAddedToken);
	//		deviceWatcher.Updated(deviceWatcherUpdatedToken);
	//		deviceWatcher.Removed(deviceWatcherRemovedToken);
	//		deviceWatcher.EnumerationCompleted(deviceWatcherEnumerationCompletedToken);
	//		deviceWatcher.Stopped(deviceWatcherStoppedToken);

	//		if (deviceWatcher.Status() != winrt::Windows::Devices::Enumeration::DeviceWatcherStatus::Created)
	//			deviceWatcher.Stop();
	//		deviceWatcher = nullptr;
	//	}
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::start() {
	//	devices.clear();
	//	deviceWatcher.Start();
	//	while (deviceWatcher.Status() != winrt::Windows::Devices::Enumeration::DeviceWatcherStatus::EnumerationCompleted);
	//	int k = 5;
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::stop() {
	//	deviceWatcher.Stop();
	//}

	//std::list<winrt::Windows::Devices::Enumeration::DeviceInformation> WinBluetoothDeviceDiscoveryAgent::getDiscoveredDevices() const {
	//	return devices;
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::device_Added(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformation deviceInfo) {
	//	if (sender == deviceWatcher) {
	//		if (!deviceInfo.Name().empty()) {
	//			devices.push_back(deviceInfo);
	//			QSharedPointer<QListWidgetItem> item = QSharedPointer<QListWidgetItem>(new QListWidgetItem(QString(winrt::to_string(deviceInfo.Name()).c_str())));
	//			item->setForeground(QColor(Qt::black));
	//			listWidget->addItem(item.data());
	//		}
	//	}
	//}

	//void WinBluetoothDeviceDiscoveryAgent::device_Updated(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate) {
	//	if (sender == deviceWatcher) {
	//		/*auto it = std::find_if(devices.begin(), devices.end(), [&](const winrt::Windows::Devices::Bluetooth::BluetoothDevice& device) {
	//			return device.BluetoothDeviceId().Id() == deviceInfoUpdate.Id();
	//			}
	//		);
	//		if (it != devices.end())
	//			it->Update(deviceInfoUpdate);*/
	//	}
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::device_Removed(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate) {
	//	if (sender == deviceWatcher) {
	//		/*auto it = std::find_if(devices.begin(), devices.end(), [&](const winrt::Windows::Devices::Bluetooth::BluetoothDevice& device) {
	//			return device.BluetoothDeviceId().Id() == deviceInfoUpdate.Id();
	//			}
	//		);
	//		if (it != devices.end()) {
	//			
	//		}*/
	//	}
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::device_EnumerationCompleted(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&) {
	//	if (sender == deviceWatcher) {

	//	}
	//}

	//// TODO
	//void WinBluetoothDeviceDiscoveryAgent::device_Stopped(winrt::Windows::Devices::Enumeration::DeviceWatcher sender, winrt::Windows::Foundation::IInspectable const&) {
	//	if (sender == deviceWatcher) {

	//	}
	//}

	//// TODO Normal radio initilization without std::thread
	//WinBluetoothManager::WinBluetoothManager(QListWidget* listWidget, QPushButton* button) :
	//	listWidget(listWidget),
	//	discoveryAgent(listWidget),
	//	radio(nullptr),
	//	state(PowerState::Off)
	//{
	//	// The reason for creating the thread:
	//	// https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/concurrency#:~:text=Calling%20get%20makes,a%20different%20technique.
	//	std::thread initilizeRadioThread(&WinBluetoothManager::initilizeRadio, this);
	//	initilizeRadioThread.join();
	//}

	//// TODO Check the functionality of the bluetooth module
	//void WinBluetoothManager::initilizeRadio() {
	//	using winrt::Windows::Devices::Radios::RadioState;
	//	using winrt::Windows::Devices::Bluetooth::BluetoothAdapter;
	//	radio = BluetoothAdapter::GetDefaultAsync().get().GetRadioAsync().get();
	//	if (radio.State() == RadioState::On)
	//		state = PowerState::On;
	//	else if (radio.State() == RadioState::Off)
	//		state = PowerState::Off;
	//}

	//// TODO Normal setting state without std::thread
	//void WinBluetoothManager::setPowerState(const PowerState& state) {
	//	try {
	//		// The reason for creating the thread:
	//		// https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/concurrency#:~:text=Calling%20get%20makes,a%20different%20technique.
	//		//std::thread setStateTask(cringe, std::ref(radio), state);
	//		//setStateTask.join();
	//		std::thread setStateThread(&WinBluetoothManager::setPowerStateWinRT, this, state);
	//		setStateThread.join();
	//		this->state = state;
	//	}
	//	catch (winrt::hresult_error const& e) {
	//		qDebug() << winrt::to_string(e.message()).c_str() << '\n';
	//	}
	//}

	//// TODO RadioAcessStatus handling
	//void WinBluetoothManager::setPowerStateWinRT(const PowerState& state) {
	//	using winrt::Windows::Devices::Radios::RadioState;
	//	auto accessStatus = radio.SetStateAsync((state == PowerState::On) ? RadioState::On : RadioState::Off).get();
	//}

	//void cringe(winrt::Windows::Devices::Radios::Radio& radio, const PowerState& state) {
	//	using winrt::Windows::Devices::Radios::RadioState;
	//	radio.SetStateAsync((state == PowerState::On) ? RadioState::On : RadioState::Off).get();
	//}

	//PowerState WinBluetoothManager::getPowerState() {
	//	return state;
	//}

	//WinBluetoothDeviceDiscoveryAgent WinBluetoothManager::getDiscoveryAgent() const {
	//	return discoveryAgent;
	//}

	//winrt::Windows::Devices::Radios::Radio initilizeRadio() {
	//	return winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync().get().GetRadioAsync().get();
	//}

	//void initilizeRadio(std::promise<winrt::Windows::Devices::Radios::Radio>&& p) {
	//	p.set_value(winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync().get().GetRadioAsync().get());
	//}

	// TODO RadioAcessStatus handling
	//winrt::Windows::Foundation::IAsyncAction WinBluetoothManager::setStateAsync(const PowerState& state) {
	//	using winrt::Windows::Devices::Radios::RadioState;
	//	try {
	//		std::lock_guard<std::mutex> lock(mutex);	// I don't know why but it works
	//		//QMutexLocker locker(&mutex);
	//		//winrt::apartment_context ui_thread;
	//		//co_await winrt::resume_background();
	//		//auto accessStatus = radio.SetStateAsync((state == PowerState::On) ? RadioState::On : RadioState::Off);
	//		//accessStatus.get();
	//		//co_await ui_thread;
	//		auto accessStatus{ co_await radio.SetStateAsync((state == PowerState::On) ? RadioState::On : RadioState::Off) };
	//	} catch (winrt::hresult_error const& e) {
	//		qDebug() << e.message().c_str() << '\n';
	//	}
	//}

	//winrt::Windows::Foundation::IAsyncAction TryGetRadio(winrt::Windows::Devices::Radios::Radio& radio) {
	//	auto ba = co_await winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync();
	//	radio = co_await ba.GetRadioAsync();
	//	co_await radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::On);
	//}

	// TODO Check the functionality of the bluetooth module
	//winrt::Windows::Foundation::IAsyncAction WinBluetoothManager::initilizeRadio() {
	//	using winrt::Windows::Devices::Radios::RadioState;
	//	auto bluetoothAdapter = co_await winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync();
	//	radio = co_await bluetoothAdapter.GetRadioAsync();
	//	if (radio.State() == RadioState::On)
	//		state = PowerState::On;
	//	else if (radio.State() == RadioState::Off)
	//		state = PowerState::Off;
	//}
