// Common utilities for all CERT C rules are in crate::utility::cert_c

// Implemented rules with nested structure
#[path = "API/API00-C/api00_c.rs"]
pub mod api00_c;

#[path = "API/API01-C/api01_c.rs"]
pub mod api01_c;

#[path = "API/API02-C/api02_c.rs"]
pub mod api02_c;

#[path = "API/API04-C/api04_c.rs"]
pub mod api04_c;

#[path = "API/API05-C/api05_c.rs"]
pub mod api05_c;

#[path = "ARR/ARR00-C/arr00_c.rs"]
pub mod arr00_c;

#[path = "ARR/ARR01-C/arr01_c.rs"]
pub mod arr01_c;

#[path = "ARR/ARR30-C/arr30_c.rs"]
pub mod arr30_c;

#[path = "ARR/ARR32-C/arr32_c.rs"]
pub mod arr32_c;

#[path = "ARR/ARR36-C/arr36_c.rs"]
pub mod arr36_c;

#[path = "ARR/ARR37-C/arr37_c.rs"]
pub mod arr37_c;

#[path = "ARR/ARR38-C/arr38_c.rs"]
pub mod arr38_c;

#[path = "ARR/ARR39-C/arr39_c.rs"]
pub mod arr39_c;

#[path = "DCL/DCL00-C/dcl00_c.rs"]
pub mod dcl00_c;

#[path = "ERR/ERR33-C/err33_c.rs"]
pub mod err33_c;

#[path = "EXP/EXP15-C/exp15_c.rs"]
pub mod exp15_c;

#[path = "EXP/EXP33-C/exp33_c.rs"]
pub mod exp33_c;

#[path = "EXP/EXP34-C/exp34_c.rs"]
pub mod exp34_c;

#[path = "FIO/FIO30-C/fio30_c.rs"]
pub mod fio30_c;

#[path = "FIO/FIO34-C/fio34_c.rs"]
pub mod fio34_c;

#[path = "FIO/FIO37-C/fio37_c.rs"]
pub mod fio37_c;

#[path = "INT/INT18-C/int18_c.rs"]
pub mod int18_c;

#[path = "INT/INT30-C/int30_c.rs"]
pub mod int30_c;

#[path = "INT/INT32-C/int32_c.rs"]
pub mod int32_c;

#[path = "MEM/MEM30-C/mem30_c.rs"]
pub mod mem30_c;

#[path = "MEM/MEM31-C/mem31_c.rs"]
pub mod mem31_c;

#[path = "MEM/MEM33-C/mem33_c.rs"]
pub mod mem33_c;

#[path = "MSC/MSC32-C/msc32_c.rs"]
pub mod msc32_c;

#[path = "POS/POS30-C/pos30_c.rs"]
pub mod pos30_c;

#[path = "POS/POS36-C/pos36_c.rs"]
pub mod pos36_c;

#[path = "POS/POS37-C/pos37_c.rs"]
pub mod pos37_c;

#[path = "POS/POS54-C/pos54_c.rs"]
pub mod pos54_c;

#[path = "PRE/PRE30-C/pre30_c.rs"]
pub mod pre30_c;

#[path = "PRE/PRE31-C/pre31_c.rs"]
pub mod pre31_c;

#[path = "PRE/PRE09-C/pre09_c.rs"]
pub mod pre09_c;

#[path = "PRE/PRE32-C/pre32_c.rs"]
pub mod pre32_c;
#[path = "STR/STR30-C/str30_c.rs"]
pub mod str30_c;

#[path = "STR/STR31-C/str31_c.rs"]
pub mod str31_c;

#[path = "STR/STR38-C/str38_c.rs"]
pub mod str38_c;

#[path = "SIG/SIG30-C/sig30_c.rs"]
pub mod sig30_c;

#[path = "SIG/SIG31-C/sig31_c.rs"]
pub mod sig31_c;

use super::{CertRule, RuleRegistry};

impl RuleRegistry {
    pub fn new() -> Self {
        let mut registry = Self { rules: Vec::new() };

        registry.register(Box::new(api00_c::Api00C));
        registry.register(Box::new(api01_c::Api01C));
        registry.register(Box::new(api02_c::Api02C));
        registry.register(Box::new(api04_c::Api04C));
        registry.register(Box::new(api05_c::Api05C));
        registry.register(Box::new(arr00_c::Arr00C));
        registry.register(Box::new(arr01_c::Arr01C));
        registry.register(Box::new(arr30_c::Arr30C));
        registry.register(Box::new(arr32_c::Arr32C));
        registry.register(Box::new(arr36_c::Arr36C));
        registry.register(Box::new(arr37_c::Arr37C));
        registry.register(Box::new(arr38_c::Arr38C));
        registry.register(Box::new(arr39_c::Arr39C));
        registry.register(Box::new(dcl00_c::Dcl00C));
        registry.register(Box::new(err33_c::Err33C));
        registry.register(Box::new(exp15_c::Exp15C));
        registry.register(Box::new(exp33_c::Exp33C));
        registry.register(Box::new(exp34_c::Exp34C));
        registry.register(Box::new(fio30_c::Fio30C));
        registry.register(Box::new(fio34_c::Fio34C::new()));
        registry.register(Box::new(fio37_c::Fio37C));
        registry.register(Box::new(int18_c::Int18C));
        registry.register(Box::new(int30_c::Int30C));
        registry.register(Box::new(int32_c::Int32C));
        registry.register(Box::new(mem30_c::Mem30C));
        registry.register(Box::new(mem31_c::Mem31C));
        registry.register(Box::new(mem33_c::Mem33C::new()));
        registry.register(Box::new(msc32_c::Msc32C));
        registry.register(Box::new(pos30_c::Pos30C));
        registry.register(Box::new(pos36_c::Pos36C));
        registry.register(Box::new(pos37_c::Pos37C));
        registry.register(Box::new(pos54_c::Pos54C));
        registry.register(Box::new(pre30_c::Pre30C));
        registry.register(Box::new(pre31_c::Pre31C));
        registry.register(Box::new(pre09_c::Pre09C));
        registry.register(Box::new(pre32_c::Pre32C));
        registry.register(Box::new(str30_c::Str30C));
        registry.register(Box::new(sig31_c::Sig31C));
        registry.register(Box::new(str31_c::Str31C));
        registry.register(Box::new(str38_c::Str38C));
        registry.register(Box::new(sig30_c::Sig30C));
        registry.register(Box::new(win01_c::Win01C));
        registry.register(Box::new(win02_c::Win02C));

        registry
    }

    pub fn register(&mut self, rule: Box<dyn CertRule>) {
        self.rules.push(rule);
    }

    pub fn get_rule(&self, rule_id: &str) -> Option<&dyn CertRule> {
        self.rules
            .iter()
            .find(|rule| rule.rule_id() == rule_id)
            .map(|rule| rule.as_ref())
    }

    pub fn all_rules(&self) -> &[Box<dyn CertRule>] {
        &self.rules
    }
}

// Integration tests module
#[cfg(test)]
mod integration;

#[path = "WIN/WIN01-C/win01_c.rs"]
pub mod win01_c;

#[path = "WIN/WIN02-C/win02_c.rs"]
pub mod win02_c;
