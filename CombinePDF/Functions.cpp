#include "Functions.h"

#include "qpdf/DLL.h"
#include "qpdf/QPDF.hh"
#include "qpdf/QPDFWriter.hh"

#include <Windows.h>
#include <stdio.h>

#pragma comment(lib, "qpdf.lib")

ProgressParams progress;
QPDF output_file;
QPDFWriter output = QPDFWriter(output_file);

using callback_fn = DWORD(*)(ProgressParams* params);

callback_fn callback;

class ProgressReporter : public QPDFWriter::ProgressReporter
{
public:
	ProgressReporter()
	{
	}
	virtual ~ProgressReporter()
	{
	}

	void reportProgress(int percentage)
	{
		progress.percentage = percentage;

		if (callback(&progress) == 1)
			_exit(1);
	}
};

extern "C" int _declspec(dllexport) qpdf_combine_files(const wchar_t* input_filenames, const wchar_t* output_filename, void* callback_function)
{
	if (!lstrlenW(input_filenames))
		return RETURN_INPUTFILENAMES_IS_EMPTY;

	if (!lstrlenW(output_filename))
		return RETURN_OUTPUTFILENAME_IS_EMPTY;

	const wchar_t* cutted_input_filenames = input_filenames;
	bool first_page_added = false;

	callback = (callback_fn)callback_function;

	// Изначально output_file это набор белеберды от выделенной памяти
	// Поэтому делаем из неё пустой PDF, чтобы не было проблем с добавлением страниц
	output_file.emptyPDF();

	while (*cutted_input_filenames)
	{
		int performed_pdfs = 1;
		
		char wchar2char[MAX_PATH + 1];
		std::wcstombs(wchar2char, cutted_input_filenames, MAX_PATH + 1);

		QPDF temp_file;
		temp_file.processFile(wchar2char);

		const std::vector<QPDFObjectHandle>& input_file_pages = temp_file.getAllPages();
		
		// Копируем в выходной файл страницы из входного 
		for (const QPDFObjectHandle& input_file_n_page : input_file_pages)
		{
			output_file.addPage(input_file_n_page, !first_page_added);
			first_page_added = true;
		}

		if (callback_function)
		{
			progress.file_idx = performed_pdfs;
			progress.file_name = cutted_input_filenames;
			progress.file_pages = input_file_pages.size();

			int callback_result = callback(&progress);

			if (callback_result == 1)
				return RETURN_ABORTED_BY_CALLBACK;
		}

		performed_pdfs++;

		// Переходим к следующему пути файла обходя нуль-терминатор с помощью единицы
		cutted_input_filenames += wcslen(cutted_input_filenames) + 1;
	}

	char wchar2char[MAX_PATH + 1];
	std::wcstombs(wchar2char, output_filename, MAX_PATH + 1);

	output = QPDFWriter(output_file, wchar2char);

	if (callback_function)
	{
		output.registerProgressReporter(PointerHolder<ProgressReporter>(new ProgressReporter()).getPointer());

		progress.state = QPDF_WRITING;
		progress.file_name = output_filename;
		progress.file_pages = output_file.getAllPages().size();
		progress.file_idx += 1;

		int callback_result = callback(&progress);

		if (callback_result == 1)
			return RETURN_ABORTED_BY_CALLBACK;
	}

	output.write();

	return RETURN_OK;
}
