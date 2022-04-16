#include "Helper.h"

bool CheckResult(const HRESULT& arg_result, ID3DBlob* arg_errorBlob)
{
	if (FAILED(arg_result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(arg_errorBlob->GetBufferSize());

		std::copy_n((char*)arg_errorBlob->GetBufferPointer(),
			arg_errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	return true;
}

