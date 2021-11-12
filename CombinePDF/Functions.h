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
	int state = 0; // ��� ������ ����� 0, ��� ������ ����� - 1
	int	file_idx = 1; // ���������� ����� ��������� �����
	int	file_pages = 0; // ���������� ������� � �����
	int percentage = 0; // �������� � ���������

	const wchar_t* file_name = L""; // ��� ��������� ��� ������������ �����
};

extern "C" int _declspec(dllexport) qpdf_combine_files(const wchar_t* input_filenames, const wchar_t* output_filename, void* callback_function = nullptr);