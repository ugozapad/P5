
#include "PCH.h"
#include "MRenderGL_Context.h"

void CRenderContextGL::OcclusionQuery_Init()
{
	for(int i = 0; i < OCID_HASHFRAMES; i++)
		m_OC_lQueryIDHash[i].Create(128, OCID_HASHNUM, false);

	m_OC_iHashQuery = 0;
	m_OC_iHashInsert = 1;
}

void CRenderContextGL::OcclusionQuery_PrepareFrame()
{
	m_OC_iHashQuery = m_OC_iHashInsert;
	m_OC_iHashInsert++;
	if (m_OC_iHashInsert >= OCID_HASHFRAMES)
		m_OC_iHashInsert = 0;

	m_OC_lQueryIDHash[m_OC_iHashInsert].Create(128, OCID_HASHNUM, false);
	m_OC_iHashOCIDNext[m_OC_iHashInsert] = 1;
}

int CRenderContextGL::OcclusionQuery_AddQueryID(int _QueryID)
{
	if (m_OC_iHashOCIDNext[m_OC_iHashInsert] >= 128)
		return 0;

	m_OC_lQueryIDHash[m_OC_iHashInsert].Insert(m_OC_iHashOCIDNext[m_OC_iHashInsert], _QueryID);

	int OCID = m_OC_iHashOCIDNext[m_OC_iHashInsert] + m_OC_iHashInsert*128;
	m_OC_iHashOCIDNext[m_OC_iHashInsert]++;

	return OCID;
}

int CRenderContextGL::OcclusionQuery_FindOCID(int _QueryID)
{
	return m_OC_lQueryIDHash[m_OC_iHashQuery].GetOCID(_QueryID) + m_OC_iHashQuery*128;
}

void CRenderContextGL::OcclusionQuery_Begin(int _QueryID)
{
	int OCID = OcclusionQuery_AddQueryID(_QueryID);
	if (!OCID)
		Error("OcclusionQuery_Begin", "Fixme!");

	if (m_AttribChanged) Attrib_Update();
	if (m_MatrixChanged) Matrix_Update();

	glBeginQuery(GL_SAMPLES_PASSED, OCID);
	GLESErr("OcclusionQuery_Begin");
}

void CRenderContextGL::OcclusionQuery_End()
{
	// Fix: _QueryID not necessary

	glEndQuery(GL_SAMPLES_PASSED);
	GLESErr("OcclusionQuery_End");
}

int CRenderContextGL::OcclusionQuery_GetVisiblePixelCount(int _QueryID)
{
	int OCID = OcclusionQuery_FindOCID(_QueryID);
	if (OCID < 0)
		return 0;

	if (!glIsQuery(OCID))
		return 0;

	int bIsQueryAvail = 0;
	glGetQueryiv(OCID, GL_QUERY_RESULT_AVAILABLE, &bIsQueryAvail);
	if (!bIsQueryAvail)
	{
		CMTime T;
		TStart(T);

		while(!bIsQueryAvail)
		{
			glGetQueryiv(OCID, GL_QUERY_RESULT_AVAILABLE, &bIsQueryAvail);
		}

		TStop(T);

		if (T.GetTime() > 0.001f)
			ConOutL(CStrF("(GL) Block on occlusion query: %.2f ms.", T.GetTime()*1000.0));
	}

	int nPixels = 0;
	glGetQueryiv(OCID, GL_QUERY_RESULT, &nPixels);
//ConOut(CStrF("Query %d, VisPixels %d", _QueryID, nPixels));
	return nPixels;
}

