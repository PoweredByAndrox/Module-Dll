#include "..\Dll\Dll.h"

// CURRENT VERSION IS DEFINE "SUPPORT_VERSION"

#define CURRENT_VERSION_TEXT "0.0.0"
#define CURRENT_VERSION 0x00000000L

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
	std::function<T(Args)> GetFunctionDLL(const std::string &path, const std::string& func_name);

	// Load Dll And Starts Whole Functions In Dll
	std::pair<void *, void *> LoadDLL(const std::string &path = "..\\Debug\\Dll.dll",
		std::pair<void *, void *> Args = { (void *)"test_args_for_init", (void *)"test_args_for_start" });

	// Detach required DLL from array
	// Id sets only in LoadDLL function in array
	void DetachDLL(const std::string &id);
private:
	std::map<std::string, HINSTANCE> Dlls;

	const std::string InitFuncName = "init_dll", StartFuncName = "start_dll", version = "get_version", support_version = "get_suport_version";
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
	if (path.empty())
	{
		MessageBoxA(0, "Library Path Is Empty! Aborting", "Error", MB_OK);
		return nullptr;
	}
	else if (func_name.empty())
	{
		MessageBoxA(0, "Function Name Is Empty! Aborting", "Error", MB_OK);
		return nullptr;
	}

	std::string path_id = path.substr(path.find_last_of('\\') + 1);

	auto Dll = Dlls.find(path_id);

	//typedef T(WINAPI *def_func)(Args);
	typedef T(WINAPIV *def_func)(Args);
	HINSTANCE newDll = nullptr;

	if (Dll != Dlls.end())
	{
		newDll = (*Dll).second;
	}
	else
	{
		newDll = ::LoadLibraryA(path.c_str());
	}
	if (!newDll)
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
			if (Dlls.find(path_id) == Dlls.end())
				Dlls.insert({ path_id, newDll });

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

	if (path.empty())
	{
		MessageBoxA(0, "Library Path Is Empty! Aborting", "Error", MB_OK);
		return { nullptr, nullptr };
	}

	std::string path_id = path.substr(path.find_last_of('\\') + 1);

	if (!Dlls.empty() && Dlls.find(path_id) != Dlls.end())
	{
		MessageBoxA(0, ("Current library '" + version + "' already loaded in system!").c_str(), "Error", MB_OK);
	}
	else
	{
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

			typedef long(WINAPIV *def_ver_func)();
			def_ver_func VersionDLL = (def_ver_func)::GetProcAddress(newDll, version.c_str());
			def_ver_func SupportVersionDLL = (def_ver_func)::GetProcAddress(newDll, support_version.c_str());

			// check if new dll have functions of versions and supported this exe and not below version support
			if (VersionDLL && SupportVersionDLL)
			{
				long SupportVersion = (*SupportVersionDLL)();

				if (CURRENT_VERSION >= SupportVersion)
				{
					typedef void *(WINAPIV *def_func)(void *);
					def_func InitFunc = (def_func)::GetProcAddress(newDll, InitFuncName.c_str());
					def_func StartFunc = (def_func)::GetProcAddress(newDll, StartFuncName.c_str());

					if (InitFunc && StartFunc)
					{
						Dlls.insert({ path_id, newDll });

						MessageBoxA(0, ("Version Of DLL Is: '" + std::to_string((*VersionDLL)()) +
							"', Supported Version Is: '" + std::to_string(SupportVersion) + "'").c_str(), "INFO", MB_OK);

						return { (*InitFunc)(Args.first), (*StartFunc)(Args.second) };
					}
					else
					{
						MessageBoxA(0, ("Functions '" + InitFuncName + "' And '" + StartFuncName + "' from '" + path + "' not found, error text: "
							+ GetLastErrorAsString()).c_str(), "Error", MB_OK);
					}
				}
				else
				{
					MessageBoxA(0, ("Fail Checking Verion Dll, This DLL Is Not Supported By This Version Exe!\nThis Version Exe: '" +
						std::to_string(CURRENT_VERSION) + "', Support Version DLL: '" + std::to_string(SupportVersion) + "'!").c_str(), "Error", MB_OK);
				}
			}
			else
			{
				MessageBoxA(0, ("Functions '" + version + "' And '" + support_version + "' from '" + path + "' not found, error text: "
					+ GetLastErrorAsString()).c_str(), "Error", MB_OK);
			}
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

	double x = 0.0,
		y = 0.0,
		result = 0.0;
	char oper = '+';

	std::cout << "Calculator Console Application" << std::endl << std::endl;
	std::cout << "Please enter the operation to perform. Format: a+b | a-b | a*b | a/b" << std::endl;

	while (true)
	{
		std::cin >> x >> oper >> y;
		if (oper == '/' && y == 0)
		{
			std::cout << "Division by 0 exception" << std::endl;
			continue;
		}
		else
		{
			auto Func = M_Dll->GetFunctionDLL<void *, void *>("..\\Debug\\Dll.dll", "start_dll");
			if (Func)
			{
				MessageBoxA(0, ("Func Return Value: " +
					std::to_string(*(double *)Func((void *)std::vector<double>{x, (double) oper, y}.data()))).c_str(),
					"Done!", MB_OK);
				result = *(double *)Func((void *)std::vector<double>{x, (double)oper, y}.data());
			}
		}
		std::cout << "Result is: " << result << std::endl;
	}

	/*
	*/
	return 0;
}