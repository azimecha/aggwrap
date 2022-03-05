#include "Pen.hpp"
#include <agg_path_storage.h>
#include <agg_conv_curve.h>
#include <agg_conv_stroke.h>

using namespace AGGWrap;

AGGWrap::Pen::Pen(SharedObject<Brush>& rsoBrush, const AwPenParams_t& rpar) : m_rsoBrush(rsoBrush) {
	m_rsoBrush.AddRef();
	m_par = rpar;
}

AGGWrap::Pen::Pen(const Pen& rpen) : m_rsoBrush(rpen.m_rsoBrush) {
	m_rsoBrush.AddRef();
	m_par = rpen.m_par;
}

AGGWrap::Pen::~Pen(void) {
	m_rsoBrush.RemoveRef();
}

void AGGWrap::Pen::StrokePath(const Path& rpathSource, Path& rpathDest) const {
	agg::path_storage storage(rpathSource.GetStorage());
	agg::conv_curve<agg::path_storage> convCurve(storage);
	agg::conv_stroke<agg::conv_curve<agg::path_storage>> convStroke(convCurve);

	convStroke.width(m_par.fThickness);
	rpathDest.AddAGGPath(convStroke);
}

AGGWRAP_EXPIMPL AwPen_h AGGWRAP_FUNC AwCreatePen(AwBrush_h hBrush, AwPenParams_pc pcParams) {
	try {
		return (AwPen_h)new Pen(*(SharedObject<Brush>*)hBrush, *pcParams);
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwGetPenParams(AwPen_h hPen, AGGWRAP_OUT AwPenParams_p pParams) {
	try {
		*pParams = ((Pen*)hPen)->GetParams();
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeletePen(AwPen_h hPen) {
	try {
		delete (Pen*)hPen;
	} catch (...) { }
}
