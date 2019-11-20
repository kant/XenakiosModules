#include "clocks.h"

extern std::shared_ptr<Font> g_font;

RandomClockModule::RandomClockModule()
{
    config(17,1,9);
    configParam(0,0.0f,1.0f,0.1); // master clock density
    float defmult = rescale(1.0f,0.1f,10.0f,0.0f,1.0f);
    for (int i=0;i<8;++i)
    {
        configParam(i+1,0.0f,1.0f,defmult); // clock multiplier
        // gate len
        // >=0.0 && <=0.5 deterministic percentage 1% to 99% of clock interval
        // >0.5 && <=1.0 stochastic distribution favoring short and long values
        configParam(i+9,0.0,1.0f,0.25f); 
    }
}

void RandomClockModule::process(const ProcessArgs& args)
{
    float masterdensity = params[0].getValue();
    if (masterdensity<0.5f)
    {
        masterdensity = rescale(masterdensity,0.0f,0.5f,0.0f,1.0f);
        masterdensity = pow(masterdensity,0.7f);
        masterdensity = rescale(masterdensity,0.0f,1.0f,0.05f,1.0f);
    }
    else
    {
        masterdensity = rescale(masterdensity,0.5f,1.0f,0.0f,1.0f);
        masterdensity = pow(masterdensity,4.0f);
        masterdensity = rescale(masterdensity,0.0f,1.0f,1.0f,200.0f);
    }
    m_curDensity = masterdensity;
    
    for (int i=0;i<8;++i)
    {
        if (outputs[i].isConnected())
        {
            float multip = rescale(params[i+1].getValue(),0.0f,1.0f,0.1f,10.0f);
            m_clocks[i].setDensity(masterdensity*multip);
            float glen = params[i+9].getValue();
            m_clocks[i].setGateLen(glen);
            outputs[i].setVoltage(10.0f*m_clocks[i].process(args.sampleTime));    
        }
    }
    outputs[8].setVoltage(m_clocks[0].getCurrentGateLen()*5.0);
}

RandomClockWidget::RandomClockWidget(RandomClockModule* m)
{
    if (!g_font)
    	g_font = APP->window->loadFont(asset::plugin(pluginInstance, "res/sudo/Sudo.ttf"));
    setModule(m);
    box.size.x = 100;
    m_mod = m;
    for (int i=0;i<8;++i)
    {
        addOutput(createOutput<PJ301MPort>(Vec(5,30+30*i), module, i));
        addParam(createParam<RoundSmallBlackKnob>(Vec(35, 30+30*i), module, i+1)); 
        addParam(createParam<RoundSmallBlackKnob>(Vec(65, 30+30*i), module, i+9)); 
    }
    addParam(createParam<RoundBlackKnob>(Vec(5, 30+30*8), module, 0));    
    addOutput(createOutput<PJ301MPort>(Vec(45,30+30*8), module, 8));
}

void RandomClockWidget::draw(const DrawArgs &args)
{
    
    nvgSave(args.vg);
    float w = box.size.x;
    float h = box.size.y;
    nvgBeginPath(args.vg);
    nvgFillColor(args.vg, nvgRGBA(0x80, 0x80, 0x80, 0xff));
    nvgRect(args.vg,0.0f,0.0f,w,h);
    nvgFill(args.vg);

    nvgFontSize(args.vg, 15);
    nvgFontFaceId(args.vg, g_font->handle);
    nvgTextLetterSpacing(args.vg, -1);
    nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
    nvgText(args.vg, 3 , 10, "Random clock", NULL);
    char buf[100];
    if (m_mod)
        sprintf(buf,"Xenakios %f",m_mod->m_curDensity);
    else
    {
        sprintf(buf,"Xenakios");
    }
    
    nvgText(args.vg, 3 , h-11, buf, NULL);
    nvgRestore(args.vg);
    ModuleWidget::draw(args);
}

DividerClockWidget::DividerClockWidget(DivisionClockModule* m)
{
    if (!g_font)
    	g_font = APP->window->loadFont(asset::plugin(pluginInstance, "res/sudo/Sudo.ttf"));
    setModule(m);
    box.size.x = 110;
    for (int i=0;i<8;++i)
    {
        addParam(createParam<RoundSmallBlackKnob>(Vec(3, 30+30*i), module, i)); 
        addParam(createParam<RoundSmallBlackKnob>(Vec(35, 30+30*i), module, i+8)); 
        addOutput(createOutput<PJ301MPort>(Vec(70,30+30*i), module, i));
    }
    
}

void DividerClockWidget::draw(const DrawArgs &args)
{
    nvgSave(args.vg);
    float w = box.size.x;
    float h = box.size.y;
    nvgBeginPath(args.vg);
    nvgFillColor(args.vg, nvgRGBA(0x80, 0x80, 0x80, 0xff));
    nvgRect(args.vg,0.0f,0.0f,w,h);
    nvgFill(args.vg);

    nvgFontSize(args.vg, 15);
    nvgFontFaceId(args.vg, g_font->handle);
    nvgTextLetterSpacing(args.vg, -1);
    nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
    nvgText(args.vg, 3 , 10, "Divider clock", NULL);
    
    
    nvgText(args.vg, 3 , h-11, "Xenakios", NULL);
    nvgRestore(args.vg);
    ModuleWidget::draw(args);
}