////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"
#include "IArchivable.h"

// stl
#include <string>
#include <vector>
#include <array>
#include <utility> // for integer_sequence

#include <iostream>

namespace emll
{
namespace model
{
    // Variadic helper class to transform input types into a std::tuple<std::vector<T1>, std::vector<T2>, ...>
    template <typename... Types>
    struct TupleOfVectors
    {
        using type = std::tuple<std::vector<Types>...>;
    };

    template <typename TupleType, size_t... Sequence>
    auto MakeTupleOfVectorsFromTypeTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        return typename TupleOfVectors<typename std::tuple_element<Sequence, TupleType>::type...>::type();
    }

    template <typename TupleType>
    auto MakeTupleOfVectorsFromTypeTuple(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromTypeTupleHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    // TupleType is a tuple<PortElements<T1>, PortElements<T2>, ...>
    template <typename TupleType, size_t... Sequence>
    auto MakeTupleOfVectorsFromPortElementsTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        return typename TupleOfVectors<typename std::tuple_element<Sequence, TupleType>::type::type...>::type();
    }

    template <typename TupleType>
    auto MakeTupleOfVectorsFromPortElementsTuple(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromPortElementsTupleHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    template <typename TupleType>
    struct TupleVectorMaker
    {
        using type = decltype(MakeTupleOfVectorsFromTypeTuple(TupleType{}));
    };

    template <typename TupleType>
    struct TupleVectorMakerFromPortElements
    {
        using type = decltype(MakeTupleOfVectorsFromPortElementsTuple(TupleType{}));
    };

    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename InputTypesTuple, typename OutputTypesTuple>
    class Map
    {
    public:
        Map(const Model& model,
            const InputTypesTuple& inputs,
            const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
            const OutputTypesTuple& outputs,
            const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputTypesTuple, OutputTypesTuple>("Map"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        const Model& GetModel() const { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        void Refine(const TransformContext& context);

        // <summary> Set inputs </summary>
        template <typename... InputNodeTypes>
        void SetInputs(const std::tuple<std::vector<InputNodeTypes>...>& inputValues);

        // TODO: have SetInputs forward its args to tuple constructors (a la make_tuple)


        /// <summary> Computes the output of the map from its current input values </summary>
        typename TupleVectorMakerFromPortElements<OutputTypesTuple>::type Compute() const;

    private:
        Model _model;

        InputTypesTuple _inputs; // this is a tuple of InputNode<T>*
        std::array<std::string, std::tuple_size<InputTypesTuple>::value> _inputNames;

        OutputTypesTuple _outputs; // This is a tuple of PortElements<T>
        std::array<std::string, std::tuple_size<OutputTypesTuple>::value> _outputNames;

        // Remap
        template <typename OutputElementsType>
        void RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer);

        template <size_t... Sequence>
        void RemapOutputElementsHelper(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer);

        // SetInput
        template <typename InputNodeType, typename InputType>
        void SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues);

        template <typename InputNodesTupleType, size_t... Sequence>
        void SetInputElementsHelper(std::index_sequence<Sequence...>, const InputNodesTupleType& inputValues);

        // Compute
        template <typename PortElementsType, typename OutputType>
        void ComputeElements(PortElementsType& elements, OutputType& output) const;

        template <size_t... Sequence>
        void ComputeElementsHelper(std::index_sequence<Sequence...>, typename TupleVectorMakerFromPortElements<OutputTypesTuple>::type& outputValues) const;
    };

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto MakeMap(const Model& model,
                 const InputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                 const OutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);
}
}

#include "../tcc/Map.tcc"