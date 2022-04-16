#include "Helper.h"

bool CheckResult(const HRESULT& arg_result, ID3DBlob* arg_errorBlob)
{
	if (FAILED(arg_result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(arg_errorBlob->GetBufferSize());

		std::copy_n((char*)arg_errorBlob->GetBufferPointer(),
			arg_errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	return true;
}

