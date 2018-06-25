use codegen::values::NumValue;
use codegen::{globals, BlockContext};
use inkwell::values::PointerValue;
use mir::block::Global;

pub fn gen_global_statement(global: &Global, node: &mut BlockContext) -> PointerValue {
    let num_val = NumValue::new_undef(node.ctx.context, node.ctx.allocb);
    let vec_ptr = match global {
        Global::SampleRate => globals::get_sample_rate(node.ctx.module),
        Global::BPM => globals::get_bpm(node.ctx.module),
    };
    let vec_val = node.ctx
        .b
        .build_load(&vec_ptr, "global.vec")
        .into_vector_value();
    num_val.set_vec(node.ctx.b, &vec_val);
    num_val.val
}
