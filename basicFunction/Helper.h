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
/// 生成などのチェック
/// </summary>
/// <param name="arg_result">リザルト</param>
/// <param name="arg_errorBlob">エラー</param>
/// <returns>生成できたか</returns>
bool CheckResult(const HRESULT& arg_result, ID3DBlob* arg_errorBlob);

