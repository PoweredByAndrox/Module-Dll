#include "..\Dll\Dll.h"

// Useful links below:
//https://stackoverflow.com/questions/65802990/convert-stdfunctionvoid-to-void
//https://stackoverflow.com/questions/10079625/c-run-time-check-failure-0-the-value-of-esp-was-not-properly-saved-across-a
//https://stackoverflow.com/questions/11657968/how-to-get-list-of-functions-inside-a-dll-managed-and-unmanaged
//http://www.codenet.ru/progr/visualc/Using-DLL.php
// 

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
const std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

class ManagerDLL
{
public:
	ManagerDLL();
	~ManagerDLL();

	// T - is defined that what's the type needed to be used
	// Args - is what args of nededed function to get from DLL
	// Return NULL was failed loading DLL or function wasn't found in Library
	template<class T, class Args = void>
	std::function<T(Args)> GetFunctionDLL(const std::string &path = "G:\\Test_Dlls\\Debug\\Dll.dll",
		const std::string& func_name = "test_api");

	// Load Dll And Starts Whole Functions In Dll
	std::pair<void *, void *> LoadDLL(const std::string &path = "G:\\Test_Dlls\\Debug\\Dll.dll",
		std::pair<void *, void *> Args = { (void *)"test_args_for_init", (void *)"test_args_for_start" });

	// Detach required DLL from array
	// Id sets only in LoadDLL function in array
	void DetachDLL(const std::string &id);
private:
	std::map<std::string, HINSTANCE> Dlls;

	const std::string InitFuncName = "init_dll", StartFuncName = "start_dll";
};

ManagerDLL::ManagerDLL()
{
}

ManagerDLL::~ManagerDLL()
{
	if (!Dlls.empty())
	{
		for (auto &dll : Dlls)
		{
			DetachDLL(dll.first);
		}

		Dlls.clear();
	}
}

void ManagerDLL::DetachDLL(const std::string &id)
{
	auto try_find = Dlls.find(id);
	if (try_find != Dlls.end())
	{
		auto &dll = *try_find;
		if (!dll.first.empty() && dll.second)
		{
			::FreeLibrary(dll.second);
		}
	}
}

template<class T, class Args>
std::function<T(Args)> ManagerDLL::GetFunctionDLL(const std::string &path, const std::string &func_name)
{
	//typedef T(WINAPI *def_func)(Args);
	typedef T(WINAPIV *def_func)(Args);
	HINSTANCE newDll;
	if (!(newDll = ::LoadLibraryA(path.c_str())))
	{
		MessageBoxA(0, ("Library from '" + path + "' don't load, error text: " + GetLastErrorAsString()).c_str(), "Error", MB_OK);
	}
	else
	{
		/* this is when our function was linked with __stdcall (WINAPI define)
		* 
		* def_func someFunc = (def_func)::GetProcAddress(hMyDll, ("_" + func_name + "@4").c_str());
		*/
		
		def_func someFunc = (def_func)::GetProcAddress(newDll, func_name.c_str());
		if (someFunc)
		{
			Dlls.insert({ path.substr(path.find_last_of('\\') + 1), newDll });
			return std::function<T(Args)>{ *someFunc };
		}
		else
		{
			MessageBoxA(0, ("Function '" + func_name + "' from '" + path + "' not found, error text: "
				+ GetLastErrorAsString()).c_str(), "Error", MB_OK);
		}
	}
	return 0;
}

std::pair<void *, void *> ManagerDLL::LoadDLL(const std::string &path, std::pair<void *, void *> Args)
{
	HINSTANCE newDll;
	if ((newDll = ::LoadLibraryA(path.c_str())) == 0)
	{
		MessageBoxA(0, ("Library from '" + path + "' don't load, error text: " + GetLastErrorAsString()).c_str(), "Error", MB_OK);
	}
	else
	{
		/* this is when our function was linked with __stdcall (WINAPI define)
		*
		* def_func pfnMyFunction = (def_func)::GetProcAddress(hMyDll, ("_" + func_name + "@4").c_str());
		*/

		typedef void *(WINAPIV *def_func)(void *);
		def_func InitFunc = (def_func)::GetProcAddress(newDll, InitFuncName.c_str());
		def_func StartFunc = (def_func)::GetProcAddress(newDll, StartFuncName.c_str());

		if (InitFunc && StartFunc)
		{
			Dlls.insert({ path.substr(path.find_last_of('\\') + 1), newDll });

			return { (*InitFunc)(Args.first), (*StartFunc)(Args.second)};
		}
		else
		{
			MessageBoxA(0, ("Functions '" + InitFuncName + "' And '" + StartFuncName + "' from '" + path + "' not found, error text: "
				+ GetLastErrorAsString()).c_str(), "Error", MB_OK);
		}
	}
	return { nullptr, nullptr };
}

int main()
{
	std::unique_ptr<ManagerDLL> M_Dll = std::make_unique<ManagerDLL>();

	auto ret = M_Dll->LoadDLL();
	if (!ret.first)
	{
		MessageBoxA(0, "LoadDLL InitFunc Return nullptr!", "Unsuccessful!", MB_OK);
	}
	if (!ret.second)
	{
		MessageBoxA(0, "LoadDLL StartFunc Return nullptr!", "Unsuccessful!", MB_OK);
	}

	/*
		auto Func = M_Dll->GetFunctionDLL<int, const std::string &>();
		if (Func)
		{
			MessageBoxA(0, ("Func Return Value: " + std::to_string(Func(std::string("Test")))).c_str(), "Done!", MB_OK);
		}
	*/
	return 0;
}