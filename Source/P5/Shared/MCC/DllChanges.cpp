// -> MMath

L�gg in exlicit p� CVec3Dfp32(fp32) constructor.

Destructor till CDataFile som anropar Close


static CMat4Dfp32 ParseRotMatString(CStr _s)
{
	CMat4Dfp32 Mat;

	char* pStr = (char*) _s;
	if(!pStr)
	{
		Mat.Unit();
		return Mat;
	}

	CQuatfp32 Q;
	CVec3Dfp32 V;
	
	int pos = 0;
	int len = _s.Len();
	for(int i = 0; i < 4; i++)
	{
		pos = CStr::GoToDigit(pStr, pos, len);
		Q.k[i] = atof(&pStr[pos]);
		pos = CStr::SkipADigit(pStr, pos, len);
	}
	for(int j = 0; j < 3; j++)
	{
		pos = CStr::GoToDigit(pStr, pos, len);
		V.k[j] = atof(&pStr[pos]);
		pos = CStr::SkipADigit(pStr, pos, len);
	}

	Q.Normalize();
	Q.CreateMatrix(Mat);
	V.SetMatrixRow(Mat, 3);
	return Mat;
}

static CStr GetRotMatString(CMat4Dfp32 &_Mat)
{
	CQuatfp32 Q;

	Q.Create(_Mat);
	const CVec3Dfp32 &V = CVec3Dfp32::GetMatrixRow(_Mat, 3);

	return CStrF("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
		Q.k[0], Q.k[1], Q.k[2], Q.k[3], V[0], V[1], V[2]);	
}

static void InterpolateMatrix(const CMat4Dfp32 &_M0, const CMat4Dfp32 &_M1, fp32 _T, CMat4Dfp32 &_Res)
{
	CVec3Dfp32 VRes;
	CVec3Dfp32::GetMatrixRow(_M0, 3).Lerp(CVec3Dfp32::GetMatrixRow(_M1, 3), _T, VRes);

	CQuatfp32 q1, q2;
	q1.Create(_M0);
	q2.Create(_M1);

	CQuatfp32 QRes;
	q1.Interpolate(q2, QRes, _T);

	QRes.CreateMatrix(_Res);
	VRes.SetMatrixRow(_Res, 3);
}

// -> MFile

static CStr GetFullPath(CStr _Path)
{
	CFStr OldDir;
	_getcwd(OldDir, OldDir.GetMax());

	CStr Directory = _Path.GetPath();
	if(Directory.Len())
		if(_chdir(Directory) != 0)
			return _Path;

	CStr CurDir = CStrF(' ', 256);
	_getcwd(CurDir, 256);

	_chdir(OldDir);

	return CurDir + "\\" + _Path.GetFilename();
}

