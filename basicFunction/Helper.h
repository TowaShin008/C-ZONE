#pragma once
#include<d3d12.h>
#include<string>
class Helper
{
public:
	Helper();
	~Helper();
};

/// <summary>
/// �����Ȃǂ̃`�F�b�N
/// </summary>
/// <param name="arg_result">���U���g</param>
/// <param name="arg_errorBlob">�G���[</param>
/// <returns>�����ł�����</returns>
bool CheckResult(const HRESULT& arg_result, ID3DBlob* arg_errorBlob);

