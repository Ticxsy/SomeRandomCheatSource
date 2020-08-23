#pragma once
#include <stdio.h>
#include <windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <D3dx9math.h>
#include <type_traits>
#include <locale>

#include <MinHook.h>
#pragma comment(lib, "minhook.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

ImGuiWindow &BeginScene();
VOID EndScene(ImGuiWindow &window);

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Color
{
public:

	RGBA NiggaGreen = { 128, 224, 0, 200 };
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 0,255,0,255 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,160 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };
	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };
	RGBA Plum = { 221,160,221,160 };

};
Color Col;

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}
};

bool DataCompare(PBYTE pData, PBYTE bSig, char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bSig)
	{
		if (*szMask == 'x' && *pData != *bSig)
			return false;
	}
	return (*szMask) == 0;
}

PBYTE FindPattern(PBYTE dwAddress, DWORD dwSize, PBYTE pbSig, char* szMask, long offset)
{
	size_t length = strlen(szMask);
	for (size_t i = NULL; i < dwSize - length; i++)
	{
		if (DataCompare(dwAddress + i, pbSig, szMask))
			return dwAddress + i + offset;
	}
	return nullptr;
}

namespace detail
{
    extern "C" void* _spoofer_stub();

    template <typename Ret, typename... Args>
    static inline auto shellcode_stub_helper(
        const void* shell,
        Args... args
    ) -> Ret
    {
        auto fn = (Ret(*)(Args...))(shell);
        return fn(args...);
    }

    template <std::size_t Argc, typename>
    struct argument_remapper
    {
        // At least 5 params
        template<
            typename Ret,
            typename First,
            typename Second,
            typename Third,
            typename Fourth,
            typename... Pack
        >
            static auto do_call(
                const void* shell,
                void* shell_param,
                First first,
                Second second,
                Third third,
                Fourth fourth,
                Pack... pack
            ) -> Ret
        {
            return shellcode_stub_helper<
                Ret,
                First,
                Second,
                Third,
                Fourth,
                void*,
                void*,
                Pack...
            >(
                shell,
                first,
                second,
                third,
                fourth,
                shell_param,
                nullptr,
                pack...
                );
        }
    };

    template <std::size_t Argc>
    struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
    {
        // 4 or less params
        template<
            typename Ret,
            typename First = void*,
            typename Second = void*,
            typename Third = void*,
            typename Fourth = void*
        >
            static auto do_call(
                const void* shell,
                void* shell_param,
                First first = First{},
                Second second = Second{},
                Third third = Third{},
                Fourth fourth = Fourth{}
            ) -> Ret
        {
            return shellcode_stub_helper<
                Ret,
                First,
                Second,
                Third,
                Fourth,
                void*,
                void*
            >(
                shell,
                first,
                second,
                third,
                fourth,
                shell_param,
                nullptr
                );
        }
    };
}


template <typename Ret, typename... Args>
static inline auto spoof_call(
    const void* trampoline,
    Ret(*fn)(Args...),
    Args... args
) -> Ret
{
    struct shell_params
    {
        const void* trampoline;
        void* function;
        void* rbx;
    };

    shell_params p{ trampoline, reinterpret_cast<void*>(fn) };
    using mapper = detail::argument_remapper<sizeof...(Args), void>;
    return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
}

template<class T>
struct TArray {
	friend struct FString;

public:
	inline TArray() {
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const {
		return Count;
	};

	inline T& operator[](int i) {
		return Data[i];
	};

	inline const T& operator[](int i) const {
		return Data[i];
	};

	inline bool IsValidIndex(int i) const {
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};


struct FString : public TArray<wchar_t>
{
	__forceinline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? (int32_t)std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	__forceinline bool IsValid() const
	{
		return Data != nullptr;
	}

	__forceinline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};