#include "Application.hpp"

using namespace Wasteland;

int main()
{
	try
	{
		Application::GetInstance().PreInitialize();
		Application::GetInstance().Initialize();

		while (Application::GetInstance().IsRunning())
		{
			Application::GetInstance().Update();
			Application::GetInstance().Render();
		}

		Application::GetInstance().Uninitialize();
	}
	catch (const BaseException& exception)
	{
		BaseException::OnException(exception);
	}

	return 0;
}