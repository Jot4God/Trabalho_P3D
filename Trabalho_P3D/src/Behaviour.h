#pragma once


// ==============================================================
// Ficheiro:    Behaviour.h
// Descrição:   Declaração da classe Behaviour
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Não possui dependências externas.
// Observações:
// Esta classe serve como uma 'interface' base para comportamentos que podem ser aplicados a objetos no jogo.
// ==============================================================


namespace game_engine_p3d {

	// Declaração antecipada (forward declaration) da classe Object.
	// Necessária para evitar dependências circulares entre Behaviour e Object.
	// Nota que não é necessário incluir aqui o cabeçalho Object.h, pois apenas declaramos que existe uma classe Object.
	// Isso permite que Behaviour conheça o tipo Object sem precisar conhecer sua implementação completa.
	class Object;


	class Behaviour {
	public:
		// ------------------------------------------------------------
		// Construtores e destrutores
		// ------------------------------------------------------------
		// Destrutor padrão virtual
		// Indica que o compilador deve gerar automaticamente a implementação de um destrutor padrão.
		// É uma alternativa à definição de um destrutor com corpo de função vazio, i.e., Behaviour() {}
		virtual ~Behaviour() = default;

		// ------------------------------------------------------------
		// Funções que definem o comportamento do objeto em diferentes fases do ciclo de vida)
		// ------------------------------------------------------------
		// Funções virtuais
		// Estas funções podem ser sobrescritas por classes derivadas para alterar o seu comportamento em tempo de execução.
		// Usar 'override' na classe derivada ajuda a garantir que a sobrescrição é intencional.
		virtual void Start(Object& obj) {}
		virtual void Update(Object& obj) {}
		virtual void FixedUpdate(Object& obj) {}
		virtual void PhysicsUpdate(Object& obj) {}
		virtual void LateUpdate(Object& obj) {}
		virtual void OnCollisionEnter(Object& obj, Object& other) {}
		virtual void OnCollisionExit(Object& obj, Object& other) {}
		virtual void OnTriggerEnter(Object& obj, Object& other) {}
		virtual void OnTriggerExit(Object& obj, Object& other) {}
		virtual void OnDestroy(Object& obj) {}
	};
}