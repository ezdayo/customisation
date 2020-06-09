/**
 *
 * @file      src/component.cpp
 *
 * @brief     This is the implementation of a Customisation component
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#include "customisation/component.hpp"
#include "customisation/entity.hpp"

#undef LOGTAG
#define LOGTAG "Customisation::Component"
#include "log.hpp"

namespace Customisation {

static const std::string invalid("Invalid");

Component Component::INVALID(invalid, Trait::NONE, Trait::NONE);

Component::Component(const std::string &type,
                     Trait base, Trait mask) noexcept :
    _name(), _description(), _type(type), _traits(Trait::UNDEFINED|base),
    _mask(mask & ~Trait::UNDEFINED), _owner(nullptr) {}

Entity &Component::entity() noexcept {
    if ((_traits & (Trait::UNDEFINED | Trait::ENTITY)) == Trait::ENTITY) {
        return *(static_cast<Entity *>(this));
    }

    return Entity::INVALID;
}
        
const Entity &Component::entity() const noexcept {
    if ((_traits & (Trait::UNDEFINED | Trait::ENTITY)) == Trait::ENTITY) {
        return *(static_cast<const Entity *>(this));
    }
    return Entity::INVALID;
}
      
const Entity *Component::owner() const noexcept {
    return _owner;
}        

Component &Component::denominate(const std::string name) noexcept {
    ASSERT(_name.empty(), "Cannot change the name of the component: %s",
           _name.c_str());
    _name = std::move(name);
    return *this;
}
        
Component &Component::describe(const std::string desc) noexcept {
    ASSERT(_description.empty(), "Cannot re-describe the component: %s ('%s')",
           _name.c_str(), _description.c_str());
    _description = std::move(desc);
    return *this;
}

Component &Component::characterise(const Trait traits) noexcept {
    ASSERT(((_traits & Trait::UNDEFINED) == Trait::UNDEFINED),
            "Cannot re-characterise the component: %s (%08x)",
           _name.c_str(), static_cast<uint32_t>(_traits));

    LOGD_IF_NOT(((traits & ~_mask) == Trait::NONE),
                "Some traits cannot be set on component: %s (%08x)",
                _name.c_str(), static_cast<uint32_t>(_traits & ~_mask));

    _traits = (_traits & ~(_mask|Trait::UNDEFINED)) | (traits & _mask);
    return *this;
}
        
Component &Component::bind(const Entity &owner) noexcept {
    ASSERT((_owner == nullptr),
            "Cannot bind the component to another entity: %s (%s)",
           _name.c_str(), _owner->name().c_str());

    _owner = &owner;
    return *this;
}
        
const std::string &Component::name() const noexcept {
    return _name;
}
        
const std::string &Component::description() const noexcept {
    return _description;
}
        
const std::string &Component::type() const noexcept {
    return _type;
}
        
Trait Component::traits() const noexcept {
    return _traits;
}
        
Error Component::set(const std::string &val) noexcept {
    if ((_traits & (Trait::UNDEFINED | Trait::SETTABLE)) == Trait::SETTABLE) {
        return string_to_value(val);
    }
    return Error::INVALID_REQUEST;
}
        
Error Component::configure(const std::string &val) noexcept {
    if ((_traits & (Trait::UNDEFINED | Trait::CONFIGURABLE | Trait::LOCKED)) == 
        Trait::CONFIGURABLE) {
        return string_to_value(val);
    }
    return Error::INVALID_REQUEST;
}

Error Component::get(std::string &into) const noexcept {
    if ((_traits & Trait::UNDEFINED) == Trait::NONE) {
        into.append(value_to_string());
        return Error::NONE;
    }
    return Error::INVALID_REQUEST;
}
       
std::string Component::value() const noexcept {
    if ((_traits & Trait::UNDEFINED) == Trait::NONE) {
        return value_to_string();
    }
    ASSERT(false, "Accessing value of an undefined component %s",
           _name.c_str());
    return "";
}

std::string Component::values() const noexcept {
    if ((_traits & Trait::UNDEFINED) == Trait::NONE) {
        return values_to_string();
    }
    ASSERT(false, "Accessing values of an undefined component %s",
           _name.c_str());
    return "";
}

bool Component::locked() const noexcept {
    return (_traits & Trait::LOCKED) == Trait::LOCKED;
}
        
bool Component::unlocked() const noexcept {
    return (_traits & Trait::LOCKED) == Trait::NONE;
}
        
void Component::lock() noexcept {
    _traits |= Trait::LOCKED;
}
        
void Component::unlock() noexcept {
    _traits &= ~Trait::LOCKED;
}

std::string Component::value_to_string() const noexcept {
    /* It has to be defined in child classes */
    return invalid;
}
        
std::string Component::values_to_string() const noexcept {
    /* It has to be defined in child classes */
    return invalid;
}
        
Error Component::string_to_value(const std::string &/*val*/) noexcept {
    /* It has to be defined in child classes */
    return Error::UNDEFINED;
}

}  // namespace Customisation
