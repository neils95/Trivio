namespace Trivio.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddManyToManyRelationship : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.UserTriviaHistories",
                c => new
                    {
                        UserId = c.Int(nullable: false),
                        TriviaId = c.Int(nullable: false),
                        UserAction = c.Int(nullable: false),
                    })
                .PrimaryKey(t => new { t.UserId, t.TriviaId })
                .ForeignKey("dbo.Trivias", t => t.TriviaId, cascadeDelete: true)
                .ForeignKey("dbo.Users", t => t.UserId, cascadeDelete: true)
                .Index(t => t.UserId)
                .Index(t => t.TriviaId);
            
        }
        
        public override void Down()
        {
            DropForeignKey("dbo.UserTriviaHistories", "UserId", "dbo.Users");
            DropForeignKey("dbo.UserTriviaHistories", "TriviaId", "dbo.Trivias");
            DropIndex("dbo.UserTriviaHistories", new[] { "TriviaId" });
            DropIndex("dbo.UserTriviaHistories", new[] { "UserId" });
            DropTable("dbo.UserTriviaHistories");
        }
    }
}
