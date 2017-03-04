#include "pch.h"
#include "Game.h"
#pragma region using_namespace

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;



ref class App sealed :public IFrameworkView {	
	bool WindowClosed, WindowChanged;
	CGame Game;
public:
		
	
	// Inherited via IFrameworkView
	virtual void Initialize(CoreApplicationView ^applicationView)
	{	//declare the generic event for OnActive function (callback)
		applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
		CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &App::Suspending);
		CoreApplication::Resuming += ref new EventHandler<Object^>(this, &App::Resuming);
		
		WindowClosed = false;
		
		
	}
	virtual void SetWindow(CoreWindow ^window){
		
		window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::Close);
		window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::PointerPressed);
		window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::KeyDown);
		window->KeyUp += ref new Windows::Foundation::TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &App::KeyUp);
		window->SizeChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowSizeChangedEventArgs ^>(this, &App::OnSizeChanged);

	}
	virtual void Load(String ^entryPoint){}
	virtual void Run(){
		WindowChanged = false;
		Game.Initialize();
		CoreWindow^ Window = CoreWindow::GetForCurrentThread(); //return the pointer to Window 

		while (!WindowClosed|| !WindowChanged) {
			Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneIfPresent);

			Game.Update();
			Game.Render();
		}
		
	}
	virtual void Uninitialize(){}

	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {		
		CoreWindow^ Window = CoreWindow::GetForCurrentThread(); //return the pointer to Window 
		Window->Activate();		
	}

	void Close(CoreWindow^ window, CoreWindowEventArgs^ Args) {
		WindowClosed = true;
	}

	void Suspending(Object^ Sender, SuspendingEventArgs^ Args) {

	}

	void Resuming(Object^ Sender, Object^ Args) {

	}

	void PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args) {
		
		MessageDialog Dialog("Okienko {1}","Notice");
		Dialog.ShowAsync();
		
	}

	void KeyDown(CoreWindow^ Window, KeyEventArgs^ Args) {
		if (Args->VirtualKey == VirtualKey::Escape) {
			WindowClosed = true;
		}
	}
	void  KeyUp(CoreWindow^ Window, KeyEventArgs^ Args) {

	}

	

	void OnSizeChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^args);
};

ref class AppSource sealed : IFrameworkViewSource {
public:
	virtual IFrameworkView^ CreateView() {
		return ref new App();
	}
};

[MTAThread]

int main(Array<String^>^ args) {
	CoreApplication::Run(ref new AppSource());
	return 0;
}

#pragma endregion



void App::OnSizeChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^args)
{
	
	Game.Resize();
}
