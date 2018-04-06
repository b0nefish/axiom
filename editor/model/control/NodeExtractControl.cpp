#include "NodeExtractControl.h"

#include "compiler/runtime/Control.h"
#include "compiler/runtime/ControlGroup.h"

using namespace AxiomModel;

NodeExtractControl::NodeExtractControl(Node *node, size_t index, MaximRuntime::Control *runtime, ConnectionSink::Type baseType,
                                       QPoint pos, QSize size)
    : NodeControl(node, index, runtime, pos, size), m_sink(runtime, baseType) {
    initSink();

    connect(&m_sink, &ExtractConnectionSink::activeSlotsChanged,
            this, &NodeExtractControl::activeSlotsChanged);
}

void NodeExtractControl::doRuntimeUpdate() {
    m_sink.setActiveSlots(runtime()->group()->getActiveFlags());
}