#pragma once

enum progress_states
{
	QPDF_READING,
	QPDF_WRITING
};

enum return_codes
{
	RETURN_OK,
	RETURN_ABORTED_BY_CALLBACK,
	RETURN_INPUTFILENAMES_IS_EMPTY,
	RETURN_OUTPUTFILENAME_IS_EMPTY,
};

struct ProgressParams
{
	int state = 0; // при чтении файла 0, при записи файла - 1
	int	file_idx = 1; // порядковый номер читаемого файла
	int	file_pages = 0; // количество страниц в файле
	int percentage = 0; // прогресс в процентах

	const wchar_t* file_name = L""; // имя читаемого или сохраняемого файла
};

extern "C" int _declspec(dllexport) qpdf_combine_files(const wchar_t* input_filenames, const wchar_t* output_filename, void* callback_function = nullptr);